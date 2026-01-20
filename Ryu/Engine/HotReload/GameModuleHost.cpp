#include "Engine/HotReload/GameModuleHost.h"
#include "Core/Utils/Timing/FrameTimer.h"
#include "Engine/Services/Services.h"
#include "Core/Logging/Logger.h"
#include "Core/Config/CVar.h"

namespace Ryu::Engine
{
	static Config::CVar<std::string> cv_modulePath(
		"Game.ModulePath",
		"",
		"Path to Game dll");

	GameModuleHost::GameModuleHost() = default;
	
	GameModuleHost::~GameModuleHost()
	{
		Unload();
	}
	
	std::expected<void, ModuleLoadError> GameModuleHost::Load(const fs::path& dllPath)
	{
		if (m_state.IsLoaded)
		{
			Unload();
		}

		m_dllPath = dllPath;

		m_dllCopyPath = dllPath.parent_path() / (dllPath.stem().string() + "_loaded.dll");
		
		std::error_code ec;
		fs::copy_file(dllPath, m_dllCopyPath, fs::copy_options::overwrite_existing, ec);
		if (ec)
		{
			do {
    ::Ryu::Logging::Logger::Get().Error(::Ryu::ExtractFilenameNoExt("D:\\ARNAV\\DEV\\Ryu\\Ryu\\Engine\\HotReload\\GameModuleHost.cpp"), "Failed to copy DLL: {}", ec.message());
} while (0);
			return std::unexpected(ModuleLoadError::FileNotFound);
		}

		// Copy PDB if exists(for debugging)
		auto pdbPath     = dllPath.parent_path() / (dllPath.stem().string() + ".pdb");
		auto pdbCopyPath = dllPath.parent_path() / (dllPath.stem().string() + "_loaded.pdb");
		if (fs::exists(pdbPath))
		{
			fs::copy_file(pdbPath, pdbCopyPath, fs::copy_options::overwrite_existing, ec);
		}

		auto result = LoadDLL(m_dllCopyPath);
		if (!result)
		{
			return result;
		}

		if (!ResolveExports())
		{
			UnloadDLL();
			return std::unexpected(ModuleLoadError::MissingExports);
		}

		if (!ValidateVersion())
		{
			UnloadDLL();
			return std::unexpected(ModuleLoadError::VersionMismatch);
		}

		// Call module's load function
		if (!m_state.API.LoadModule(ServiceLocator::Get()))
		{
			UnloadDLL();
			return std::unexpected(ModuleLoadError::InitializationFailed);
		}

		// Get module info
		m_state.Info = m_state.API.GetModuleInfo();

		RYU_LOG_INFO("Loaded game module: {} v{}.{}.{}",
			m_state.Info.Name,
			m_state.Info.Version.Major,
			m_state.Info.Version.Minor,
			m_state.Info.Version.Patch);

		m_state.IsLoaded = true;

		// Start file watcher if auto-reload enabled
		if (m_autoReloadEnabled)
		{
			StartFileWatcher();
		}

		return {};
	}
	
	void GameModuleHost::Unload()
	{
		// NOTE: It is users responsibility to shutdown the module before shutting down the engine

		if (!m_state.IsLoaded)
		{
			return;
		}

		StopFileWatcher();

		RYU_LOG_INFO("Unloading game module");

		if (m_state.GameState)
		{
			m_state.API.DestroyGameState(m_state.GameState);
			m_state.GameState = nullptr;
		}

		if (m_state.API.UnloadModule)
		{
			m_state.API.UnloadModule();
		}

		UnloadDLL();

		// Clean up copied DLL
		std::error_code ec;
		fs::remove(m_dllCopyPath, ec);

		m_state = {};
	}
	
	std::expected<void, ModuleLoadError> GameModuleHost::Reload()
	{
		if (!m_state.IsLoaded)
		{
			return Load(m_dllPath);
		}

		RYU_LOG_INFO("Hot-reloading game module...");

		// Serialize current state
		std::vector<uint8_t> stateData;
		if (m_state.API.SerializeState && m_state.GameState)
		{
			stateData = SerializeCurrentState();
			RYU_LOG_DEBUG("Serialized {} bytes of game state", stateData.size());
		}

		// Shutdown and unload
		if (m_state.GameState)
		{
			// Don't call full shutdown - just cleanup for reload
			m_state.API.DestroyGameState(m_state.GameState);
			m_state.GameState = nullptr;
		}

		if (m_state.API.UnloadModule)
		{
			m_state.API.UnloadModule();
		}

		UnloadDLL();

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// Copy new DLL
		std::error_code ec;
		fs::remove(m_dllCopyPath, ec);
		fs::copy_file(m_dllPath, m_dllCopyPath, fs::copy_options::overwrite_existing, ec);
		if (ec)
		{
			RYU_LOG_ERROR("Failed to copy new DLL: {}", ec.message());
			return std::unexpected(ModuleLoadError::FileNotFound);
		}

		// Reload
		auto result = LoadDLL(m_dllCopyPath);
		if (!result)
		{
			return result;
		}

		if (!ResolveExports())
		{
			UnloadDLL();
			return std::unexpected(ModuleLoadError::MissingExports);
		}

		if (!ValidateVersion())
		{
			UnloadDLL();
			return std::unexpected(ModuleLoadError::VersionMismatch);
		}

		if (!m_state.API.LoadModule(ServiceLocator::Get()))
		{
			UnloadDLL();
			return std::unexpected(ModuleLoadError::InitializationFailed);
		}

		m_state.Info = m_state.API.GetModuleInfo();
		m_state.IsLoaded = true;

		// Create new game state
		m_state.GameState = m_state.API.CreateGameState();

		// Deserialize state
		if (!stateData.empty() && m_state.API.DeserializeState)
		{
			if (DeserializeState(stateData))
			{
				RYU_LOG_INFO("Game state restored after hot-reload");
			}
			else
			{
				RYU_LOG_WARN("Failed to restore game state, re-initializing");
				m_state.API.Initialize(m_state.GameState);
			}
		}
		else
		{
			m_state.API.Initialize(m_state.GameState);
		}

		RYU_LOG_INFO("Hot-reload complete: {}", m_state.Info.Name);
		m_reloadPending = false;

		return {};
	}

	void GameModuleHost::EnableAutoReload(bool enable)
	{
		m_autoReloadEnabled = enable;

		if (enable && m_state.IsLoaded)
		{
			StartFileWatcher();
		}
		else
		{
			StopFileWatcher();
		}
	}
	
	bool GameModuleHost::Initialize()
	{
		if (!m_state.IsLoaded)
		{
			return false;
		}

		m_state.GameState = m_state.API.CreateGameState();
		if (!m_state.GameState)
		{
			RYU_LOG_ERROR("Failed to create game state");
			return false;
		}

		if (!m_state.API.Initialize(m_state.GameState))
		{
			RYU_LOG_ERROR("Game initialization failed");
			m_state.API.DestroyGameState(m_state.GameState);
			m_state.GameState = nullptr;
			return false;
		}

		return true;
	}
	
	void GameModuleHost::Tick(const Utils::FrameTimer& timer)
	{
		if (m_reloadPending.exchange(false))
		{
			if (auto result = Reload())
			{
				RYU_LOG_ERROR("Failed to reload game module (Error Code: {})", (u32)result.error());
			}
		}

		if (!m_state.IsLoaded || !m_state.GameState)
		{
			return;
		}

		RyuTickContext ctx
		{
			.DeltaTime   = timer.DeltaTimeF(),
			.TotalTime   = static_cast<f32>(timer.TimeSinceStart<std::chrono::seconds>()),
			.FrameNumber = timer.FrameCount(),
			.IsPaused    = false  // TODO: pause state
		};

		m_state.API.Tick(m_state.GameState, ctx);
	}
	
	void GameModuleHost::Shutdown() const
	{
		if (!m_state.IsLoaded || !m_state.GameState)
		{
			return;
		}

		m_state.API.Shutdown(m_state.GameState);
	}

	RyuWorldManager GameModuleHost::GetWorldManager() const
	{
		if (!m_state.IsLoaded || !m_state.GameState)
		{
			return nullptr;
		}

		return m_state.API.GetWorldManager(m_state.GameState);
	}

	void GameModuleHost::OnEditorAttach() const
	{
		if (m_state.IsLoaded && m_state.GameState && m_state.API.OnEditorAttach)
		{
			m_state.API.OnEditorAttach(m_state.GameState);
		}
	}

	void GameModuleHost::OnEditorDetach() const
	{
		if (m_state.IsLoaded && m_state.GameState && m_state.API.OnEditorDetach)
		{
			m_state.API.OnEditorDetach(m_state.GameState);
		}
	}

	void GameModuleHost::OnEditorRender() const
	{
		if (m_state.IsLoaded && m_state.GameState && m_state.API.OnEditorRender)
		{
			m_state.API.OnEditorRender(m_state.GameState);
		}
	}

	std::expected<void, ModuleLoadError> GameModuleHost::LoadDLL(const fs::path& dllPath)
	{
		m_dllHandle = ::LoadLibraryW(dllPath.c_str());
		if (!m_dllHandle)
		{
			RYU_LOG_ERROR("Failed to load DLL: {} (GetLastError: {})", dllPath.string(), GetLastError());
			return std::unexpected(ModuleLoadError::LoadFailed);
		}
		return {};
	}
	
	void GameModuleHost::UnloadDLL()
	{
		if (m_dllHandle)
		{
			::FreeLibrary(static_cast<HMODULE>(m_dllHandle));
			m_dllHandle = nullptr;
		}
	}
	
	bool GameModuleHost::ResolveExports()
	{
		auto getProc = [this](const char* name) -> void*
		{
			return ::GetProcAddress(static_cast<HMODULE>(m_dllHandle), name);
		};

		// Get the API function table
		auto getAPI = reinterpret_cast<decltype(&RyuGetGameModuleAPI)>(getProc("RyuGetGameModuleAPI"));
		if (!getAPI)
		{
			RYU_LOG_ERROR("Missing export: RyuGetGameModuleAPI");
			return false;
		}

		RyuGameModuleAPI* api = getAPI();
		if (!api)
		{
			RYU_LOG_ERROR("RyuGetGameModuleAPI returned null");
			return false;
		}

		m_state.API = *api;
		if (!m_state.API.GetModuleInfo   
			|| !m_state.API.LoadModule      
			|| !m_state.API.CreateGameState 
			|| !m_state.API.DestroyGameState
			|| !m_state.API.Initialize      
			|| !m_state.API.Tick            
			|| !m_state.API.Shutdown        
			|| !m_state.API.GetWorldManager)
		{
			RYU_LOG_ERROR("Game module missing required exports");
			return false;
		}

		return true;
	}

	bool GameModuleHost::ValidateVersion()
	{
		auto info = m_state.API.GetModuleInfo();

		RyuModuleVersion required = info.RequiredEngineVersion;
		EngineVersion current = CURRENT_VERSION;

		// Check major version match (breaking changes)
		if (required.Major != current.Major)
		{
			RYU_LOG_ERROR("Engine version mismatch: module requires {}.x.x, engine is {}.{}.{}",
				required.Major, current.Major, current.Minor, current.Patch);
			return false;
		}

		// Minor version: module can require older, not newer
		if (required.Minor > current.Minor)
		{
			RYU_LOG_ERROR("Engine version too old: module requires {}.{}.x, engine is {}.{}.{}",
				required.Major, required.Minor, current.Major, current.Minor, current.Patch);
			return false;
		}

		return true;
	}
	
	std::vector<byte> GameModuleHost::SerializeCurrentState()
	{
		RyuSerializedState state = m_state.API.SerializeState(m_state.GameState);

		std::vector<byte> result;
		if (state.Data && state.Size > 0)
		{
			// Include version header
			result.resize(sizeof(u32) + state.Size);
			std::memcpy(result.data(), &state.Version, sizeof(u32));
			std::memcpy(result.data() + sizeof(u32), state.Data, state.Size);

			// Free the serialized data
			if (m_state.API.FreeSerializedState)
			{
				m_state.API.FreeSerializedState(state);
			}
		}

		return result;
	}
	
	bool GameModuleHost::DeserializeState(const std::vector<byte>& data)
	{
		if (data.size() < sizeof(u32))
		{
			return false;
		}

		u32 version;
		std::memcpy(&version, data.data(), sizeof(u32));

		RyuSerializedState state
		{
			.Data = data.data() + sizeof(u32),
			.Size = data.size() - sizeof(u32),
			.Version = version
		};

		return m_state.API.DeserializeState(m_state.GameState, state);
	}
	
	void GameModuleHost::StartFileWatcher()
	{
		// Using efsw - implementation depends on your wrapper
		// Watch the original DLL path for changes
		RYU_LOG_DEBUG("[[UNIMPLEMENTED]] Starting file watcher for: {}", m_dllPath.string());

		// Pseudocode - integrate with your efsw setup
		/*
		m_fileWatcher = std::make_unique<FileWatcher>();
		m_fileWatcher->Watch(m_dllPath.parent_path(), [this](const fs::path& changed)
		{
			if (changed.filename() == m_dllPath.filename())
			{
				OnFileChanged(changed);
			}
		});
		*/
	}

	void GameModuleHost::StopFileWatcher()
	{
		RYU_LOG_DEBUG("[[UNIMPLEMENTED]] Stopping file watcher");
		// m_fileWatcher.reset();
	}

	void GameModuleHost::OnFileChanged(const fs::path& path)
	{
		RYU_LOG_INFO("Detected change in: {}", path.string());
		m_reloadPending = true;
	}
}

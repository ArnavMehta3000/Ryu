#pragma once
#include "Engine/HotReload/GameModuleABI.h"
#include <expected>
#include <filesystem>

namespace Ryu::Utils { class FrameTimer; }

namespace Ryu::Engine
{
	namespace fs = std::filesystem;

	enum class ModuleLoadError
	{
		FileNotFound,       // DLL file doesn't exist
		LoadFailed,         // LoadLibrary failed
		MissingExports,     // Required functions not found
		VersionMismatch,    // Engine version incompatible
		InitializationFailed,// LoadModule returned false
		SerializationFailed, // Failed to serialize/deserialize state
	};

	[[nodiscard]] constexpr const char* ToString(ModuleLoadError error) noexcept
	{
		switch (error)
		{
		case ModuleLoadError::FileNotFound:         return "File not found";
		case ModuleLoadError::LoadFailed:           return "DLL load failed";
		case ModuleLoadError::MissingExports:       return "Missing required exports";
		case ModuleLoadError::VersionMismatch:      return "Version mismatch";
		case ModuleLoadError::InitializationFailed: return "Initialization failed";
		case ModuleLoadError::SerializationFailed:  return "Serialization failed";
		default:                                    return "Unknown error";
		}
	}

	struct ModuleState
	{
		RyuGameState GameState{ nullptr };
		RyuGameModuleAPI API{};
		RyuModuleInfo Info{};
		bool IsLoaded{ false };
	};

	class GameModuleHost
	{
		RYU_DISABLE_COPY_AND_MOVE(GameModuleHost)
	
	public:
		GameModuleHost();
		~GameModuleHost();

		std::expected<void, ModuleLoadError> Load(const fs::path& dllPath);
		void Unload();
		inline bool IsLoaded() const noexcept { return m_state.IsLoaded; }

		std::expected<void, ModuleLoadError> Reload();
		void EnableAutoReload(bool enable);
		inline bool IsAutoReloadEnabled() const noexcept { return m_autoReloadEnabled; }

		bool Initialize();
		void Tick(const Utils::FrameTimer& timer);
		void Shutdown() const;

		RyuWorldManager GetWorldManager() const;

		void OnEditorAttach() const;
		void OnEditorDetach() const;
		void OnEditorRender() const;

		inline const RyuModuleInfo& GetModuleInfo() const noexcept { return m_state.Info; }

	private:
		std::expected<void, ModuleLoadError> LoadDLL(const fs::path& dllPath);
		void UnloadDLL();
		bool ResolveExports();
		bool ValidateVersion();

		std::vector<byte> SerializeCurrentState();
		bool DeserializeState(const std::vector<byte>& data);

		void StartFileWatcher();
		void StopFileWatcher();
		void OnFileChanged(const fs::path& path);

	private:
		ModuleState                        m_state;
		fs::path                           m_dllPath;
		fs::path                           m_dllCopyPath;
		void*                              m_dllHandle         = nullptr;
		bool                               m_autoReloadEnabled = false;
		std::atomic<bool>                  m_reloadPending     = false;
		//std::unique_ptr<class FileWatcher> m_fileWatcer;
	};
}

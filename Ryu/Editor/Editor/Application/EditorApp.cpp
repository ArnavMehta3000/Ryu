#include "Editor/Application/EditorApp.h"

#include "Core/Config/CVar.h"
#include "Core/Logging/Logger.h"
#include "Core/Profiling/Profiling.h"
#include "Editor/Application/ImGuiFonts.h"
#include "Editor/Application/ImGuiThemes.h"
#include "Game/IGameModule.h"
#include "Game/World/WorldManager.h"
#include "Graphics/Core/GfxDevice.h"
#include <ImGui/backends/imgui_impl_dx12.h>
#include <ImGui/backends/imgui_impl_win32.h>
#include <ImGui/imgui.h>


namespace Ryu::Editor
{
	namespace
	{
		static Config::CVar<bool> cv_disableImGui(
			"Editor.NoImGui", false,
			"Disables ImGui from being initialized in Editor",
			Config::CVarFlags::Debug | Config::CVarFlags::ReadOnly);

		// Handle to the original application window procedure
		WNDPROC g_originalWndProc{ nullptr };

		static void ImGui_SrvDescriptorAlloc(
			ImGui_ImplDX12_InitInfo* info,
			D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle,
			D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
		{
			if (auto* heap = static_cast<Gfx::DescriptorHeap*>(info->UserData))
			{
				Gfx::DescriptorHandle handle = heap->Allocate();

				if (handle.IsValid())
				{
					*out_cpu_desc_handle = handle.CPU;
					*out_gpu_desc_handle = handle.GPU;
				}
				else
				{
					RYU_LOG_ERROR("Failed to allocate descriptor for ImGui!");
				}
			}
			else
			{
				RYU_LOG_ERROR("Invalid user data while trying to allocate descriptor for ImGui!");
			}
		}

		static void ImGui_SrvDescriptorFree(
			ImGui_ImplDX12_InitInfo* info,
			D3D12_CPU_DESCRIPTOR_HANDLE cpu_desc_handle,
			[[maybe_unused]]  D3D12_GPU_DESCRIPTOR_HANDLE gpu_desc_handle)
		{
			if (auto* heap = static_cast<Gfx::DescriptorHeap*>(info->UserData))
			{
				// Find the index from the cpu handle, convert to full handle and free it
				u32 index = heap->FindIndex(cpu_desc_handle);
				const Gfx::DescriptorHandle handle = heap->GetHandle(index);
				heap->Free(handle);
			}
			else
			{
				RYU_LOG_ERROR("Invalid user data while trying to free descriptor for ImGui!");
			}
		}
	}

	EditorApp::EditorApp(const EditorConfig& config)
		: m_gameModulePath(config.GameModulePath)
		, m_hotReloadEnabled(config.EnableHotReload)
	{
		RYU_PROFILE_SCOPE();

		// Create and initialize window
		m_window = std::make_shared<Window::Window>(config.WindowConfig);
		App::App::InitWindow(*m_window);

		RYU_LOG_DEBUG("EditorApp created with window: {}", config.WindowConfig.Title);
	}

	EditorApp::~EditorApp() 
	{
#if defined(RYU_HOT_RELOAD)
		if (m_moduleHost)
		{
			//m_moduleHost->Unload();
			//m_moduleHost.reset();
		}
#endif
	}

	Game::WorldManager* EditorApp::GetWorldManager() noexcept
	{
#if defined(RYU_HOT_RELOAD)
		if (m_moduleHost && m_moduleHost->IsLoaded())
		{
			// GameModuleHost returns void*, need to cast
			// The World is obtained through the module's GetActiveWorld
			RYU_TODO("Proper world manager access through host")
			return nullptr;  
		}
#endif

		if (m_userApp)
		{
			return m_userApp->GetWorldManager();
		}

		return nullptr;
	}

	void EditorApp::RequestQuit()
	{
		m_isRunning = false;

		if (m_window && m_window->IsOpen())
		{
			::SendMessage(m_window->GetHandle(), WM_CLOSE, 0, 0);
		}
	}

	bool EditorApp::OnInit()
	{
		RYU_PROFILE_SCOPE();
		RYU_LOG_TRACE("Initializing editor application");

		if (!LoadGameModule())
		{
			RYU_LOG_ERROR("Failed to load game module!");
			return false;
		}

		m_isRunning = true;
		return true;
	}

	void EditorApp::OnShutdown()
	{
		RYU_PROFILE_SCOPE();
		RYU_LOG_INFO("Shutting down editor application");

#if defined(RYU_HOT_RELOAD)
		if (m_moduleHost)
		{
			m_moduleHost->Shutdown();
			m_moduleHost->Unload();
			m_moduleHost.reset();
		}
#endif

		if (m_userApp)
		{
			m_userApp->OnShutdown();
			m_userApp.reset();
		}
	}

	void EditorApp::OnTick(const Utils::FrameTimer& timer)
	{
		RYU_PROFILE_SCOPE();

		// Process window events
		if (m_window) [[likely]]
		{
			m_window->Update();
		}

#if defined(RYU_HOT_RELOAD)
		// Test a hot-reload
		if (m_window->GetInput().IsKeyDown(Window::KeyCode::Y) && m_moduleHost) [[unlikely]]
		{
			m_moduleHost->Reload();
		}

		// Hot-reload path: tick through module host
		if (m_moduleHost && m_moduleHost->IsLoaded())
		{
			m_moduleHost->Tick(timer);
		}
		else
#endif
		// Static linking path: tick user app directly
		if (m_userApp)
		{
			m_userApp->OnTick(timer);
		}
	}

#if defined(RYU_HOT_RELOAD)
	void EditorApp::TriggerReload()
	{
		if (m_moduleHost)
		{
			auto result = m_moduleHost->Reload();
			if (!result)
			{
				RYU_LOG_ERROR("Hot-reload failed: {}", ToString(result.error()));
			}
		}
	}

	void EditorApp::SetAutoReloadEnabled(bool enabled)
	{
		m_hotReloadEnabled = enabled;
		if (m_moduleHost)
		{
			m_moduleHost->EnableAutoReload(enabled);
		}
	}

	bool EditorApp::IsAutoReloadEnabled() const
	{
		return m_hotReloadEnabled;
	}
#endif

	bool EditorApp::LoadGameModule()
	{
		RYU_PROFILE_SCOPE();

#if defined(RYU_HOT_RELOAD)
		// If a game module path is specified, use dynamic loading
		if (!m_gameModulePath.empty())
		{
			return LoadGameModuleDynamic();
		}
#endif

		// Fall back to static linking
		return LoadGameModuleStatic();
	}

	bool EditorApp::LoadGameModuleStatic()
	{
		RYU_PROFILE_SCOPE();
		RYU_LOG_DEBUG("Loading game module (static linking)");

#if !defined(RYU_HOT_RELOAD)
		// Use the statically linked CreateGameModule function
		std::unique_ptr<Game::IGameModule> gm(Game::CreateGameModule());
		if (gm)
		{
			m_userApp = gm->CreateApplication(m_window);
			if (m_userApp)
			{
				m_window->SetTitle(fmt::format("Ryu Editor - {}", gm->GetName()));

				RYU_LOG_TRACE("Initializing user application");
				m_userApp->m_isRunning = m_userApp->OnInit();
				return m_userApp->m_isRunning;
			}
		}

		RYU_LOG_ERROR("Failed to create game module (static)");
		return false;
#else
		RYU_LOG_WARN("Static linking requested but RYU_HOT_RELOAD is defined");
		return false;
#endif
	}

#if defined(RYU_HOT_RELOAD)
	bool EditorApp::LoadGameModuleDynamic()
	{
		RYU_PROFILE_SCOPE();
		RYU_LOG_DEBUG("Loading game module dynamically: {}", m_gameModulePath.string());

		m_moduleHost = std::make_unique<Engine::GameModuleHost>();

		auto result = m_moduleHost->Load(m_gameModulePath);
		if (!result)
		{
			RYU_LOG_ERROR("Failed to load game DLL: {}", ToString(result.error()));
			m_moduleHost.reset();
			return false;
		}

		// Enable auto-reload if requested
		m_moduleHost->EnableAutoReload(m_hotReloadEnabled);

		// Update window title with module name
		const auto& info = m_moduleHost->GetModuleInfo();
		m_window->SetTitle(fmt::format("Ryu Editor - {}", info.Name));

		// Initialize the game
		if (!m_moduleHost->Initialize())
		{
			RYU_LOG_ERROR("Game module initialization failed");
			m_moduleHost->Unload();
			m_moduleHost.reset();
			return false;
		}

		return true;
	}
#endif

#if defined(RYU_WITH_EDITOR)
	void EditorApp::OnImGuiSetup(Gfx::Device* device)
	{
		if (cv_disableImGui)
		{
			return;
		}

		RYU_PROFILE_SCOPE();

		RYU_LOG_TRACE("Initializing ImGui");
		m_imguiHeap = std::make_unique<Gfx::DescriptorHeap>(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 64, true, "ImGui SRV Heap");

		const HWND hWnd = GetWindow()->GetHandle();
		ImGui_ImplWin32_EnableDpiAwareness();
		const f32 dpiScale = ImGui_ImplWin32_GetDpiScaleForHwnd(hWnd);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// Add fonts
		ImFontConfig fontConfig;
		fontConfig.FontDataOwnedByAtlas = false;
		fontConfig.MergeMode = false;

		auto setFontName = [&fontConfig](std::string_view name)
		{
			constexpr size_t NAME_SIZE = 40;  // From ImFontConfig			

			for (size_t i = 0; i < NAME_SIZE; i++)
			{
				fontConfig.Name[i] = '\0';
			}
			
			std::memcpy(fontConfig.Name, name.data(), std::min(name.size(), NAME_SIZE));
		};
		
		setFontName("MapleMono");
		Font::g_editorFonts[std::to_underlying(Font::Style::MapleMono)] = io.Fonts->AddFontFromMemoryTTF(
			Font::GetFontData(Font::Style::MapleMono), Font::GetFontDataSize(Font::Style::MapleMono), 0.0f, &fontConfig);

		setFontName("Roboto");
		Font::g_editorFonts[std::to_underlying(Font::Style::Roboto)] = io.Fonts->AddFontFromMemoryTTF(
			Font::GetFontData(Font::Style::Roboto), Font::GetFontDataSize(Font::Style::Roboto), 0.0f, &fontConfig);

		setFontName("Hurmit");
		Font::g_editorFonts[std::to_underlying(Font::Style::Hurmit)] = io.Fonts->AddFontFromMemoryTTF(
			Font::GetFontData(Font::Style::Hurmit), Font::GetFontDataSize(Font::Style::Hurmit), 0.0f, &fontConfig);

		ImGuiStyle& style = ImGui::GetStyle();
		style.ScaleAllSizes(dpiScale);
		style.FontScaleDpi = dpiScale;

		ImGui_ImplDX12_InitInfo info{};
		info.Device               = device->GetNativeDevice();
		info.CommandQueue         = device->GetCommandQueue()->GetNative();
		info.NumFramesInFlight    = Gfx::FRAME_BUFFER_COUNT;
		info.RTVFormat            = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.DSVFormat            = DXGI_FORMAT_UNKNOWN;
		info.SrvDescriptorHeap    = m_imguiHeap->GetNative();
		info.SrvDescriptorAllocFn = ImGui_SrvDescriptorAlloc;
		info.SrvDescriptorFreeFn  = ImGui_SrvDescriptorFree;
		info.UserData             = m_imguiHeap.get();

		bool success = ImGui_ImplWin32_Init(hWnd);
		success = ImGui_ImplDX12_Init(&info);

		Theme::Set(Theme::Style::CatpuccinMocha);
		ImGui::PushFont(Font::g_editorFonts[std::to_underlying(Font::Style::MapleMono)], 14.0f);
	}

	void EditorApp::OnImGuiFrameBegin()
	{
		if (cv_disableImGui)
		{
			return;
		}

		RYU_PROFILE_SCOPE();

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void EditorApp::OnImGuiFrameEnd(Gfx::CommandList* cmdList)
	{
		if (cv_disableImGui)
		{
			return;
		}

		RYU_PROFILE_SCOPE();

		cmdList->SetDescriptorHeap(*m_imguiHeap);

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList->GetNative());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void EditorApp::OnImGuiRender()
	{
		if (cv_disableImGui)
			return;

		RYU_PROFILE_SCOPE();

		ImGui::ShowDemoWindow();

#if defined(RYU_HOT_RELOAD)
		// Hot-reload UI panel
		if (ImGui::Begin("Hot Reload"))
		{
			if (m_moduleHost && m_moduleHost->IsLoaded())
			{
				const auto& info = m_moduleHost->GetModuleInfo();
				ImGui::Text("Module: %s", info.Name);
				ImGui::Text("Version: %u.%u.%u",
					info.Version.Major, info.Version.Minor, info.Version.Patch);

				ImGui::Separator();

				bool autoReload = m_hotReloadEnabled;
				if (ImGui::Checkbox("Auto Reload", &autoReload))
				{
					SetAutoReloadEnabled(autoReload);
				}

				if (ImGui::Button("Force Reload"))
				{
					TriggerReload();
				}
			}
			else
			{
				ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "No module loaded");
			}
		}
		ImGui::End();

		// Game's ImGui rendering (hot-reload path)
		if (m_moduleHost && m_moduleHost->IsLoaded())
		{
			m_moduleHost->OnEditorRender();
			return;  // Don't fall through to static path
		}
#endif

		// Static linking path
		if (m_userApp)
		{
			if (Game::WorldManager* manager = m_userApp->GetWorldManager())  [[likely]]
			{
				manager->OnImGuiRender();
			}
		}
	}

	void EditorApp::OnImGuiShutdown()
	{
		if (cv_disableImGui)
		{
			return;
		}

		RYU_PROFILE_SCOPE();

		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
#endif

}

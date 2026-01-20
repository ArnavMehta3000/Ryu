#pragma once
#include "Graphics/IRendererHook.h"
#include "Graphics/Core/GfxDescriptorHeap.h"
#include "Application/App/Application.h"
#include <filesystem>

#if defined(RYU_HOT_RELOAD)
#include "Engine/HotReload/GameModuleHost.h"
#endif

namespace Ryu::Editor
{
	namespace fs = std::filesystem;

	struct EditorConfig
	{
		Window::Window::Config WindowConfig = 
		{
			.Title             = "Ryu Editor",
			.CanBorderlessDrag = true,
			.Type              = Window::WindowType::Windowed
		};
		fs::path GameModulePath;
		bool EnableHotReload = false;
	};

	// Abstraction around the user application loaded via the game module
	class EditorApp 
		: public App::IApplication
		, public Gfx::IRendererHook
	{
		RYU_DISABLE_COPY_AND_MOVE(EditorApp)
	public:
		explicit EditorApp(const EditorConfig& config);
		~EditorApp() override;

		[[nodiscard]] Window::Window* GetWindow() override { return m_window.get(); }
		[[nodiscard]] Game::WorldManager* GetWorldManager() noexcept override;
		[[nodiscard]] Gfx::IRendererHook* GetRendererHook() override { return this; }

		[[nodiscard]] bool IsRunning() const override { return m_isRunning; }
		void RequestQuit() override;
		
		[[nodiscard]] bool OnInit() override;
		void OnTick(const Utils::FrameTimer& timer) override;
		void OnShutdown() override;

		// Shared pointer to user app, or nullptr if using DLL loading
		[[nodiscard]] std::shared_ptr<App::App> GetUserApp() const { return m_userApp; }

#if defined(RYU_HOT_RELOAD)
		void TriggerReload();
		void SetAutoReloadEnabled(bool enabled);
		[[nodiscard]] bool IsAutoReloadEnabled() const;
#endif

	private:
		bool LoadGameModule();
		bool LoadGameModuleStatic();

#if defined(RYU_HOT_RELOAD)
		bool LoadGameModuleDynamic();
#endif

#if defined(RYU_WITH_EDITOR)
		void OnImGuiSetup(Gfx::Device* device) override;
		void OnImGuiFrameBegin() override;
		void OnImGuiFrameEnd(Gfx::CommandList* cmdList) override;
		void OnImGuiRender() override;
		void OnImGuiShutdown() override;
#endif

	private:
		std::shared_ptr<Window::Window> m_window;
		bool m_isRunning = false;

		fs::path m_gameModulePath;
		bool m_hotReloadEnabled = false;

		// Static linking mode (legacy)
		std::shared_ptr<App::App> m_userApp;

#if defined(RYU_HOT_RELOAD)
		std::unique_ptr<Engine::GameModuleHost> m_moduleHost;
#endif
		std::unique_ptr<Gfx::DescriptorHeap> m_imguiHeap;
	};
}

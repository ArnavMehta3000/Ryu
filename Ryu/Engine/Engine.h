#pragma once
#include "Application/App/Application.h"
#include "Engine/HotReload/GameModuleHost.h"
#include "Core/Utils/Singleton.h"
#include "Graphics/Renderer.h"
#include "Game/InputManager.h"

namespace Ryu::Engine
{
	struct Services;
	static void PrintMemoryStats();

	struct EngineConfig
	{
		Window::Window::Config Window;
		fs::path               GameModulePath;
		bool                   EnableHotReload = false;
		Gfx::IRendererHook*    RendererHook    = nullptr;
	};

	class Engine : public Utils::Singleton<Engine>
	{
		RYU_SINGLETON_DECLARE(Engine);

	public:
		~Engine() = default;

		void RYU_API RunApp(App::IApplication* app, Gfx::IRendererHook* rendererHook = nullptr);

		void RYU_API Quit() const noexcept;

		[[nodiscard]] App::IApplication* GetApplication() const { return m_currentApp; }
		[[nodiscard]] Window::Window* GetAppWindow() const {  return m_currentApp ? m_currentApp->GetWindow() : nullptr;}
		[[nodiscard]] Gfx::Renderer* GetRenderer() const { return m_renderer.get(); }
		[[nodiscard]] Game::InputManager* GetInputManager() { return m_inputManager.get(); }

		void RYU_API RunApp(std::shared_ptr<App::App> app, Gfx::IRendererHook* rendererHook = nullptr);

	protected:
		Engine() {}

	private:
		bool Init(Gfx::IRendererHook* rendererHook);
		void Shutdown();
		void MainLoop();
		void OnAppResize(u32 width, u32 height) const noexcept;

	private:
		// Current application (raw pointer, lifetime managed externally (normally from the main function))
		App::IApplication* m_currentApp = nullptr;

		// Engine systems
		std::unique_ptr<Game::InputManager> m_inputManager;
		std::unique_ptr<Gfx::Renderer>      m_renderer;

		// Event listeners
		Event::ListenerHandle m_resizeListener;
		Event::ListenerHandle m_closeListener;
	};
}

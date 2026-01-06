#pragma once
#include "Application/App/Application.h"
#include "Application/Event/ListenerHandle.h"
#include "Core/Utils/Singleton.h"
#include "Graphics/Renderer.h"
#include "Game/InputManager.h"

namespace Ryu::Engine
{
	static void PrintMemoryStats();

	class Engine : public Utils::Singleton<Engine>
	{
		RYU_SINGLETON_DECLARE(Engine);

	public:
		~Engine() = default;
		
		[[nodiscard]] inline std::shared_ptr<App::App> GetApplication() const { return m_app; }
		[[nodiscard]] inline Gfx::Renderer* GetRenderer() const { return m_renderer.get(); }
		[[nodiscard]] inline Game::InputManager* GetInputManager() { return m_inputManager.get(); }
		RYU_API void Quit() const noexcept;

		void RYU_API RunApp(std::shared_ptr<App::App> app, Gfx::IRendererHook* rendererHook = nullptr);

	protected:
		Engine() {}

	private:
		bool Init(Gfx::IRendererHook* rendererHook);
		void Shutdown();
		void MainLoop();
		void OnAppResize(u32 width, u32 height) const noexcept;

	private:
		std::unique_ptr<Game::InputManager> m_inputManager;
		std::shared_ptr<App::App>           m_app;
		std::unique_ptr<Gfx::Renderer>      m_renderer;

		Event::ListenerHandle m_resizeListener;
		Event::ListenerHandle m_closeListener;
	};
}

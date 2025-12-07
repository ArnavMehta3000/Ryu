#pragma once
#include "App/Application.h"
#include "Utils/Singleton.h"
#include "Utils/ServiceLocator.h"
#include "Event/ListenerHandle.h"
#include "Graphics/Renderer.h"

namespace Ryu::Engine
{
	static void PrintMemoryStats();

	class Engine : public Utils::Singleton<Engine>
	{
		RYU_SINGLETON_DECLARE(Engine);

	public:
		~Engine() = default;
		static f64 GetEngineUpTime();
		
		inline std::shared_ptr<App::App> GetApplication() const { return m_app; }
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
		std::shared_ptr<App::App>      m_app;
		std::unique_ptr<Gfx::Renderer> m_renderer;

		Event::ListenerHandle m_resizeListener;
		Event::ListenerHandle m_closeListener;
	};
}

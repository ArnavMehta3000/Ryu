#pragma once
#include "Application/App/Application.h"
#include "Application/Event/ListenerHandle.h"
#include "Core/Utils/Singleton.h"
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
		inline Gfx::Renderer* GetRenderer() const { return m_renderer.get(); }
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

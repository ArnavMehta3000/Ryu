#pragma once
#include "Common/Common.h"
#include "Utils/Timer.h"
#include "Window/Window.h"
#include "Logger/LogCategory.h"
#include <memory>

namespace Ryu::Engine { class Engine; }

namespace Ryu::App
{
	class App
	{
		friend class Engine::Engine;
		RYU_LOG_DECLARE_CATEGORY(App);
	public:
		App() = default;
		App(const Window::Window::Config& config);  // Create an application with a window
		virtual ~App() = default;

		void Exit();

		inline void SetWindow(std::shared_ptr<Window::Window> window) { m_window = window; }
		inline NODISCARD std::shared_ptr<Window::Window> GetWindow() const { return m_window; }
		inline NODISCARD bool IsRunning() const { return m_isRunning; }

	protected:
		virtual bool OnInit() { return true; }
		virtual void OnTick(const Utils::TimeInfo&) {}
		virtual void OnShutdown() {}

	private:
		void PreInit();
		void ProcessWindowEvents();
		void PostShutdown();
		void SetupDefaultLogger();

	private:
		bool m_isRunning = false;
		std::shared_ptr<Window::Window> m_window;
	};
}

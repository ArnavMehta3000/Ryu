#pragma once
#include "Common/Common.h"
#include "App/Utils/PathManager.h"
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
		inline NODISCARD std::shared_ptr<Window::Window> GetWindow() const noexcept{ return m_window; }
		inline NODISCARD bool IsRunning() const noexcept { return m_isRunning; }
		inline NODISCARD const PathManager& GetPathManager() const noexcept{ return m_pathManager; }

	protected:
		virtual bool OnInit() { return true; }
		virtual void OnTick(const Utils::TimeInfo&) {}
		virtual void OnShutdown() {}

	private:
		void ProcessWindowEvents();

	private:
		bool m_isRunning = false;
		PathManager m_pathManager;
		std::shared_ptr<Window::Window> m_window;
	};
}

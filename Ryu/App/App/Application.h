#pragma once
#include "Common/Common.h"
#include "App/Utils/PathManager.h"
#include "Utils/Timer.h"
#include "Window/Window.h"
#include "Logging/Logger.h"
#include <memory>

namespace Ryu::Engine { class Engine; }
namespace Ryu::Editor { class EditorApp; }

namespace Ryu::App
{
	class App
	{
		friend class Engine::Engine;
		friend class Editor::EditorApp;
		RYU_LOG_DECLARE_CATEGORY(App);
	public:
		static void InitWindow(Window::Window& window);

		explicit App(std::shared_ptr<Window::Window> window);  // Use existing window
		explicit App(const Window::Window::Config& config);  // Let the application create the window
		virtual ~App();

		void Quit();

		inline NODISCARD std::shared_ptr<Window::Window> GetWindow() const noexcept{ return m_window; }
		inline NODISCARD bool IsRunning() const noexcept { return m_isRunning; }
		inline NODISCARD const PathManager& GetPathManager() const noexcept{ return m_pathManager; }

	protected:
		virtual bool OnInit() = 0;
		virtual void OnTick(const Utils::TimeInfo&) = 0;
		virtual void OnShutdown() = 0;

	private:
		void ProcessWindowEvents();

	private:
		bool                            m_isRunning = false;
		PathManager                     m_pathManager;
		std::shared_ptr<Window::Window> m_window;
	};
}

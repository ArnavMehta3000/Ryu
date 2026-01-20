#pragma once
#include "Application/App/IApplication.h"
#include "Application/Window/Window.h"
#include "Core/Logging/Logger.h"
#include "Core/Utils/Timing/FrameTimer.h"

namespace Ryu::Engine { class GameModuleHost; }

namespace Ryu::App
{
	class App : public IApplication
	{
		RYU_DISABLE_COPY(App)
		friend class Engine::Engine;
		friend class Editor::EditorApp;
		friend class Engine::GameModuleHost;
	public:
		explicit App(std::shared_ptr<Window::Window> window);
		explicit App(const Window::Window::Config& config);
		virtual ~App();

		static void InitWindow(Window::Window& window);

		// Default implementation
		[[nodiscard]] inline  virtual bool OnInit() override { m_isRunning = true; return true; }

		[[nodiscard]] inline Window::Window* GetWindow() override { return m_window.get(); }
		[[nodiscard]] inline bool IsRunning() const override { return m_isRunning; }
		void RequestQuit() override;

		[[nodiscard]] std::shared_ptr<Window::Window> GetWindowShared() const noexcept { return m_window; }

	private:
		void ProcessWindowEvents();

	private:
		bool                            m_isRunning = false;
		std::shared_ptr<Window::Window> m_window;
	};
}

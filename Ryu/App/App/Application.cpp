#include "Application.h"
#include "Profiling/Profiling.h"

namespace Ryu::App
{
	void App::InitWindow(Window::Window& window)
	{
		// Create the window if needed
		if (!window.IsOpen)
		{
			window.Create();
		}

		window.Input.EnableRawMouseInput(true);
		window.IsDarkMode = true;
	}

	App::App(std::shared_ptr<Window::Window> window)
		: m_window(window)
	{
	}

	App::App(const Window::Window::Config& config)
	{
		RYU_PROFILE_SCOPE();

		m_window = std::make_shared<Window::Window>(config);
		App::InitWindow(*m_window);
	}

	App::~App()
	{
		m_window.reset();
	}

	void App::Exit()
	{
		m_isRunning = false;
		
		// Send a close message to the window
		if (m_window && m_window->IsOpen)
		{
			::SendMessage(m_window->GetHandle(), WM_CLOSE, 0, 0);
		}
	}

	void App::ProcessWindowEvents()
	{
		// Update window events
		m_window->Update();
		m_window->ClearPendingEvents();
	}
}

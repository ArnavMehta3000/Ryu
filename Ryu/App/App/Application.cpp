#include "Application.h"
#include "App/AppConfig.h"
#include "Profiling/Profiling.h"
#include <Elos/Window/Utils/WindowExtensions.h>

namespace Ryu::App
{
	App::App(const Window::Window::Config& config)
	{
		RYU_PROFILE_SCOPE();

		m_window = std::make_shared<Window::Window>(config);
		m_window->Create();
		
		m_window->Input.EnableRawMouseInput(true);
		m_window->IsDarkMode = true;
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
		while (m_window && m_window->IsOpen)
		{
			m_window->Update();
			m_window->ClearPendingEvents();
		}
	}
}

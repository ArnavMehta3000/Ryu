#include "Application.h"
#include "Core/Profiling/Profiling.h"

namespace Ryu::App
{
	App::App(std::shared_ptr<Window::Window> window)
		: m_window(std::move(window))
	{
		RYU_ASSERT(m_window != nullptr, "Window cannot be null");
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

	void App::InitWindow(Window::Window& window)
	{
		// Create the window if needed
		if (!window.IsOpen())
		{
			window.Create();
		}

		window.GetInput().EnableRawMouseInput(true);
	}

	void App::RequestQuit()
	{
		m_isRunning = false;

		// Send a close message to the window
		if (m_window && m_window->IsOpen()) [[likely]]
		{
			::SendMessage(m_window->GetHandle(), WM_CLOSE, 0, 0);
		}
	}

	void App::ProcessWindowEvents()
	{
		// Update window events
		if (m_window)  [[likely]]
		{
			m_window->Update();
		}
	}
}

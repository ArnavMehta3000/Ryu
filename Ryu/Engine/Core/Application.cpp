#include "Application.h"
#include <Engine/Log.h>

namespace Ryu
{
	Application::Application(const Window::Config& wndConfig)
		: IApplication(wndConfig)
	{
		RYU_ENGINE_TRACE("Application constructed");
	}

	NODISCARD bool Application::OnInit()
	{
		RYU_ENGINE_TRACE("Initializing application");

		if (!m_window.Init())
		{
			RYU_ENGINE_ASSERT(m_window.GetHandle(), "Failed to create a window!");
			return false;
		}

		m_window.Show();
		RYU_ENGINE_TRACE("Created application window");

		RYU_ENGINE_TRACE("Finished initializing application");
		return true;
	}

	void Application::OnUpdate(MAYBE_UNUSED const f32 dt)
	{
	}

	void Application::OnRender()
	{

	}

	void Application::OnShutdown()
	{
		RYU_ENGINE_TRACE("Starting application shutdown");

		RYU_ENGINE_TRACE("Finished shutting down application");
	}

	void Application::OnEvent(MAYBE_UNUSED const Input::Events::OnKeyDown& event)
	{
		RYU_ENGINE_TRACE("OnKeyDown event received - {}", char(event.Key));
	}

	void Application::OnEvent(MAYBE_UNUSED const Input::Events::OnKeyUp& event)
	{
		RYU_ENGINE_TRACE("OnKeyUp event received - {}", char(event.Key));

		if (event.Key == Input::KeyCode::Escape)
		{
			Quit();
		}
	}
}
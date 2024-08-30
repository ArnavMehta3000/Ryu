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

		if (!m_window.Create())
		{
			RYU_ENGINE_FATAL("Failed to create a window!");
			return false;
		}

		RYU_ENGINE_TRACE("Created application window");

		RYU_ENGINE_TRACE("Showing application window");
		m_window.Show();

		RYU_ENGINE_TRACE("Finished initializing application");
		return true;
	}

	void Application::OnUpdate(MAYBE_UNUSED const f32 dt)
	{
		RYU_ENGINE_TRACE("Delta time {} | FPS {}", dt, (1.0f / dt))
	}

	void Application::OnRender()
	{

	}

	void Application::OnShutdown()
	{
		RYU_ENGINE_TRACE("Starting application shutdown");

		RYU_ENGINE_TRACE("Finished shutting down application");
	}
}
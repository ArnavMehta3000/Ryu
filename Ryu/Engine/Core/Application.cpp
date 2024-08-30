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

	void Application::Run()
	{
		if (!OnInit())
		{
			RYU_ENGINE_FATAL("Failed to initialize application!");
			return;
		}

		RYU_ENGINE_DEBUG("Running application");
		while (m_window.IsOpen())
		{
			m_window.PumpMessages();

			OnUpdate();
			OnRender();
		}
		RYU_ENGINE_DEBUG("Shutting down application");

		OnShutdown();
	}


	void Application::OnUpdate()
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
}
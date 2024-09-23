#include "Application.h"
#include <Engine/Internal/Log.h>

namespace Ryu
{
	Application* Application::s_instance = nullptr;

	Application* Application::Get()
	{
		if (s_instance == nullptr)
		{
			RYU_ENGINE_WARN("Application::Get() - Application instance is nullptr");
		}
		return s_instance;
	}

	Application::Application(const Window::Config& wndConfig)
		: IApplication(wndConfig)
	{
		RYU_ENGINE_TRACE("Application instance constructed");

		RYU_ENGINE_ASSERT(!s_instance, "Application instance already exists!");

		s_instance = this;
	}

	NODISCARD bool Application::OnInit()
	{
		RYU_ENGINE_TRACE("Initializing application");

		if (!m_window.Init())
		{
			RYU_ENGINE_ASSERT(m_window.GetHandle(), "Failed to create a window! Handle is nullptr!");
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

	void Application::OnRender(MAYBE_UNUSED const f32 dt)
	{

	}

	void Application::OnShutdown()
	{
		RYU_ENGINE_TRACE("Shutting down Application");
	}

	void Application::OnEvent(MAYBE_UNUSED const Events::OnKeyDown& event)
	{
		RYU_ENGINE_TRACE("OnKeyDown event received - {}", char(event.Key));
	}

	void Application::OnEvent(MAYBE_UNUSED const Events::OnKeyUp& event)
	{
		RYU_ENGINE_TRACE("OnKeyUp event received - {}", char(event.Key));

		// TODO: Remove quick exit
		if (event.Key == KeyCode::Escape)
		{
			Quit();
		}
	}

	void Application::OnEvent(MAYBE_UNUSED const Events::OnMouseButtonUp& event)
	{
		//RYU_ENGINE_TRACE("OnMouseButtonUp event received - {} {}x{}", (u32)event.Button, event.PosX, event.PosY);
	}

	void Application::OnEvent(MAYBE_UNUSED const Events::OnMouseButtonDown& event)
	{
		//RYU_ENGINE_TRACE("OnMouseButtonDown event received - {} {}x{}", (u32)event.Button, event.PosX, event.PosY);
	}

	void Application::OnEvent(MAYBE_UNUSED const Events::OnMouseDblClick& event)
	{
		//RYU_ENGINE_TRACE("OnMouseDblClick event received - {} {}x{}", (u32)event.Button, event.PosX, event.PosY);
	}

	void Application::OnEvent(MAYBE_UNUSED const Events::OnMouseMove& event)
	{
		//RYU_ENGINE_TRACE("OnMouseMove event received - {} {}x{}", (u32)event.Button, event.PosX, event.PosY);
	}

	void Application::OnEvent(MAYBE_UNUSED const Events::OnMouseMoveRaw& event)
	{
		//RYU_ENGINE_TRACE("OnMouseMoveRaw event received - {} {}x{}", (u32)event.Button, event.PosX, event.PosY);
	}

	void Application::OnEvent(MAYBE_UNUSED const Events::OnMouseWheel& event)
	{
		//RYU_ENGINE_TRACE("OnMouseWheel event received - {} | Delta: {} {}x{}", (u32)event.Button, event.Delta, event.PosX, event.PosY);
	}
}

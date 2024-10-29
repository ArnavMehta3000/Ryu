#include "Application.h"
#include <objbase.h>

namespace Ryu::App
{
	Application::Application()
		: m_isRunning(false)
		, m_hasFocus(false)
	{
	}

	Application::~Application() = default;

	bool Application::Init()
	{
		::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		
		m_window = std::make_shared<Window>(L"Ryu App", 1280, 720, this);
		m_inputSystem = std::make_unique<Input::InputSystem>(m_window->GetHWND(), this);
		
		m_window->Create();

		m_isRunning = true;

		// Call user initialization now
		return OnInit();
	}

	void Application::Shutdown()
	{
		OnShutdown();
	}

	void Application::Tick(const f64 dt)
	{
		OnTick(dt);
	}

	void Application::StopRunning()
	{
		m_isRunning = false;
	}

#pragma region WindowEvents
	void Application::OnEvent(const App::Events::OnWindowClose& event)
	{
		m_isRunning = false;
		OnWindowClose.Broadcast(event);
	}

	void Application::OnEvent(const App::Events::OnWindowStateChange& event)
	{
		OnWindowStateChange.Broadcast(event);
	}

	void Application::OnEvent(const App::Events::OnWindowResize& event)
	{
		OnWindowResize.Broadcast(event);			
	}

	void Application::OnEvent(const App::Events::OnWindowFocusChange& event)
	{
		m_hasFocus = event.Focus == WindowFocus::Gained ? true : false;
		OnWindowFocusChange.Broadcast(event);
	}
#pragma endregion

#pragma region InputEvents
	void Application::OnEvent(const Input::Events::OnKeyDown& event)
	{
		OnKeyDown.Broadcast(event);
	}

	void Application::OnEvent(const Input::Events::OnKeyUp& event)
	{
		OnKeyUp.Broadcast(event);
	}

	void Application::OnEvent(const Input::Events::OnMouseButtonUp& event)
	{
		OnMouseButtonUp.Broadcast(event);
	}

	void Application::OnEvent(const Input::Events::OnMouseButtonDown& event)
	{
		OnMouseButtonDown.Broadcast(event);
	}

	void Application::OnEvent(const Input::Events::OnMouseDblClick& event)
	{
		OnMouseDblClick.Broadcast(event);
	}

	void Application::OnEvent(const Input::Events::OnMouseMove& event)
	{
		OnMouseMove.Broadcast(event);
	}

	void Application::OnEvent(const Input::Events::OnMouseMoveRaw& event)
	{
		OnMouseMoveRaw.Broadcast(event);
	}

	void Application::OnEvent(const Input::Events::OnMouseWheel& event)
	{
		OnMouseWheel.Broadcast(event);
	}
#pragma endregion
}

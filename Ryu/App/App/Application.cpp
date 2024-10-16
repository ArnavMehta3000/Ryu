#include "Application.h"
#include <objbase.h>

namespace Ryu::App
{
	Application::Application()
		: m_isRunning(false)
		, m_hasFocus(false)
	{
	}

	Application::~Application()
	{
	}

	bool Application::Init()
	{
		::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		
		m_window = std::make_shared<Window>(L"Ryu App", 1280, 720, this);
		m_inputSystem = std::make_unique<Input::InputSystem>(m_window->GetHandle(), this);
		
		m_window->Create();

		m_isRunning = true;
		return true;
	}

	void Application::Shutdown()
	{
		::CoUninitialize();
	}

	void Application::OnTick(MAYBE_UNUSED const f64 dt)
	{
	}

#pragma region WindowEvents
	void Application::OnEvent(MAYBE_UNUSED const App::Events::OnWindowClose& event)
	{
		m_isRunning = false;
	}

	void Application::OnEvent(MAYBE_UNUSED const App::Events::OnWindowStateChange& event)
	{
	}

	void Application::OnEvent(MAYBE_UNUSED const App::Events::OnWindowResize& event)
	{
	}

	void Application::OnEvent(MAYBE_UNUSED const App::Events::OnWindowFocusChange& event)
	{
		m_hasFocus = event.Focus == WindowFocus::Gained ? true : false;
	}
#pragma endregion

#pragma region InputEvents
	void Application::OnEvent(MAYBE_UNUSED const Input::Events::OnKeyDown& event)
	{
	}

	void Application::OnEvent(MAYBE_UNUSED const Input::Events::OnKeyUp& event)
	{
	}

	void Application::OnEvent(MAYBE_UNUSED const Input::Events::OnMouseButtonUp& event)
	{
	}

	void Application::OnEvent(MAYBE_UNUSED const Input::Events::OnMouseButtonDown& event)
	{
	}

	void Application::OnEvent(MAYBE_UNUSED const Input::Events::OnMouseDblClick& event)
	{
	}

	void Application::OnEvent(MAYBE_UNUSED const Input::Events::OnMouseMove& event)
	{
	}

	void Application::OnEvent(MAYBE_UNUSED const Input::Events::OnMouseMoveRaw& event)
	{
	}

	void Application::OnEvent(MAYBE_UNUSED const Input::Events::OnMouseWheel& event)
	{
	}
#pragma endregion
}

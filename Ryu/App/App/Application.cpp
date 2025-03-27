#include "Application.h"
#include <objbase.h>

namespace Ryu::App
{
	Application::Application()
		: m_isRunning(false)
	{
	}

	Application::~Application() = default;

	bool Application::Init()
	{
		::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		
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
	
	void Application::GetWindowCreateInfo(Elos::WindowCreateInfo& outCreateInfo)
	{
		outCreateInfo.Title = "Ryu Engine";
	}
}

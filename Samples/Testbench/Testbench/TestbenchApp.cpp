#include "Testbench/TestbenchApp.h"
#include "Profiling/Profiling.h"
#include "App/AppConfig.h"

RYU_LOG_DECLARE_CATEGORY(TestbenchApp);
using namespace Ryu;

TestbenchApp::TestbenchApp(std::shared_ptr<Window::Window> window)
	: App::App(window)
{
}

bool TestbenchApp::OnInit()
{
	RYU_PROFILE_SCOPE();
	RYU_LOG_INFO(LogTestbenchApp, "Initializing Testbench App");

	if (!m_gameInput.Init())
	{
		RYU_LOG_WARN(LogTestbenchApp, "Failed to initialize GameInput");
	}

	return true;
}

void TestbenchApp::OnShutdown()
{
	RYU_PROFILE_SCOPE();
	RYU_LOG_INFO(LogTestbenchApp, "Shutting down Testbench App");
	
	m_gameInput.Shutdown();
}

void TestbenchApp::OnTick(const Ryu::Utils::TimeInfo&)
{
	m_gameInput.PollKeyboard();
	m_gameInput.PollMouse();
}

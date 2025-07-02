#include "Testbench/TestbenchApp.h"
#include "Profiling/Profiling.h"
#include "App/AppConfig.h"

using namespace Ryu;
RYU_LOG_DECLARE_CATEGORY(TestbenchApp);

TestbenchApp::TestbenchApp(std::shared_ptr<Window::Window> window)
	: App::App(window)
{
}

bool TestbenchApp::OnInit()
{
	RYU_LOG_INFO(LogTestbenchApp, "Initializing Testbench App");
	return true;
}

void TestbenchApp::OnShutdown()
{
	RYU_PROFILE_SCOPE();
	RYU_LOG_INFO(LogTestbenchApp, "Shutting down Testbench App");
}

void TestbenchApp::OnTick(const Ryu::Utils::TimeInfo&)
{
	//RYU_LOG_INFO(LogTestbenchApp, "Ticking Testbench App");
}

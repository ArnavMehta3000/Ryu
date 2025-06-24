#include "Testbench/TestbenchApp.h"
#include "Profiling/Profiling.h"
#include "App/AppConfig.h"

using namespace Ryu;

TestbenchApp::TestbenchApp(std::shared_ptr<Window::Window> window)
	: App::App(window)
{
}

bool TestbenchApp::OnInit()
{
	RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(TestbenchApp), "Initializing Testbench App");
	return true;
}

void TestbenchApp::OnShutdown()
{
	RYU_PROFILE_SCOPE();
	RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(TestbenchApp), "Shutting down Testbench App");
}

void TestbenchApp::OnTick(const Ryu::Utils::TimeInfo&)
{
	//RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(TestbenchApp), "Ticking Testbench App");
}

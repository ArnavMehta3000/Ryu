#include "Test.h"
#include "Profiling/Profiling.h"

bool TestApp::OnInit()
{
	RYU_PROFILE_SCOPE();
	return Application::OnInit();
}

void TestApp::OnShutdown()
{
	RYU_PROFILE_SCOPE();
	Application::OnShutdown();
}

void TestApp::OnTick(const Ryu::Utils::TimeInfo& timeInfo)
{
	RYU_PROFILE_SCOPE();
	Application::OnTick(timeInfo);
}

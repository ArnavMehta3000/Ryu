#include "Testbench/TestbenchApp.h"
#include "Profiling/Profiling.h"
#include "App/AppConfig.h"

bool TestbenchApp::OnInit()
{
	RYU_PROFILE_SCOPE();
	return Application::OnInit();
}

void TestbenchApp::OnShutdown()
{
	RYU_PROFILE_SCOPE();
	Application::OnShutdown();
}

void TestbenchApp::OnTick(const Ryu::Utils::TimeInfo& timeInfo)
{
	RYU_PROFILE_SCOPE();
	Application::OnTick(timeInfo);
}

void TestbenchApp::OnWindowKeyPressedEvent(MAYBE_UNUSED const Elos::Event::KeyPressed& e)
{
#if defined(RYU_BUILD_DEBUG)
	if (e.Key == Elos::KeyCode::Escape && Ryu::App::AppConfig::Get().EscToClose)
	{
		m_window->Close();
	}
#endif
}

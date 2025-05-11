#include "Test.h"
#include "Profiling/Profiling.h"
#include "App/AppConfig.h"

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

void TestApp::OnWindowKeyPressedEvent(const Elos::Event::KeyPressed& e)
{
#if defined(RYU_BUILD_DEBUG)
	if (e.Key == Elos::KeyCode::Escape && Ryu::App::AppConfig::Get().EscToClose)
	{
		m_window->Close();
	}
#endif
}

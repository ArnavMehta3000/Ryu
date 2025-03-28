#include "Test.h"
#include "Logger/Logger.h"

bool TestApp::OnInit()
{
	auto& config = TestConfig::Get();
	LOG_WARN(RYU_USE_LOG_CATEGORY(TestApp), "Config API: {}", config.RenderAPI.Get());
	config.RenderAPI.Set("OpenGL");

	return Runtime::OnInit();
}

void TestApp::OnShutdown()
{
	Runtime::OnShutdown();
}

void TestApp::OnTick(f64 dt)
{
	Runtime::OnTick(dt);
}

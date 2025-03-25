#include "Test.h"
#include "Logger/Logger.h"

bool TestApp::OnInit()
{
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

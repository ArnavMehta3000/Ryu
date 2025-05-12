#include "TestModule.h"
#include "Testbench/TestApp.h"

Ryu::App::Application* TestModule::CreateApplication()
{
	return new TestApp();
}

constexpr const char* TestModule::GetName() const
{
	return "Test App";
}

RYU_IMPLEMENT_GAME_MODULE(TestModule)

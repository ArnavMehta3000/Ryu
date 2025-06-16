#include "TestModule.h"
#include "Testbench/TestbenchApp.h"

Ryu::App::Application* TestModule::CreateApplication()
{
	return nullptr;
}

constexpr const char* TestModule::GetName() const
{
	return "Test App";
}

RYU_IMPLEMENT_GAME_MODULE(TestModule)

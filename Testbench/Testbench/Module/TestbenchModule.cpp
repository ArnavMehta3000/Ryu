#include "Testbench/Module/TestbenchModule.h"
#include "Testbench/TestbenchApp.h"

using namespace Ryu;

std::shared_ptr<App::App> TestModule::CreateApplication(std::shared_ptr<Window::Window> window)
{
	return std::make_shared<TestbenchApp>(window);
}

constexpr const char* TestModule::GetName() const
{
	return "Testbench App";
}

RYU_IMPLEMENT_GAME_MODULE(TestModule)

#include "Testbench/TestbenchModule.h"
#include "Testbench/TestbenchApp.h"

using namespace Ryu;
using namespace Ryu::Game;

RYU_IMPLEMENT_GAME_MODULE(TestbenchModule);
RYU_IMPLEMENT_HOT_RELOAD_MODULE(TestbenchModule);

std::shared_ptr<Ryu::App::App> TestbenchModule::CreateApplication(std::shared_ptr<Ryu::Window::Window> window)
{
	return std::make_shared<TestbenchApp>(window);
}

RyuModuleInfo TestbenchModule::GetModuleInfo()
{
	return RyuModuleInfo
	{
		.Name                  = AppName,
		.Author                = "Batman",
		.Version               = { 1, 0, 0 },
		.RequiredEngineVersion = { 1, 0, 0 }
	};
}

bool TestbenchModule::Initialize(RyuGameState state)
{
	return static_cast<TestbenchGameState*>(state)->Initialize();
}

void TestbenchModule::Tick(RyuGameState state, RyuTickContext ctx)
{
	static_cast<TestbenchGameState*>(state)->Tick(ctx.DeltaTime);
}

void TestbenchModule::Shutdown(RyuGameState state)
{
	static_cast<TestbenchGameState*>(state)->Shutdown();
}

RyuWorldManager TestbenchModule::GetWorldManager(RyuGameState state)
{
	return static_cast<TestbenchGameState*>(state)->GetWorldManager();
}

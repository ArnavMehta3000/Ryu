#include <Windows.h>
#include <Testbench/Application/TestApplication.h>
#include <Core/Utils/DllLoader.h>
#include <Core/Log/Log.h>
#include <Core/Log/Logger.h>
#include <Engine/Engine.h>

int WINAPI wWinMain(
	_In_     MAYBE_UNUSED HINSTANCE hInstance,
	_In_opt_ MAYBE_UNUSED HINSTANCE hPrevInstance,
	_In_     MAYBE_UNUSED LPWSTR    lpCmdLine,
	_In_     MAYBE_UNUSED int       nCmdShow)
{
	Ryu::DllLoader engineLoader;
	RYU_CORE_ASSERT(Testbench, engineLoader.LoadDLL("RyuEngine.dll"), "Failed to load RyuEngine.dll");
	RYU_CORE_ASSERT(Testbench, engineLoader.LoadProcAddress("GetEngineInstance"), "Failed to load GetEngineInstance function");

	auto getEngineInstance = engineLoader.GetProcAddressFunc<Ryu::GetEngineInstance_f>("GetEngineInstance");
	Ryu::Engine& engine = getEngineInstance();
	engine.CreateApplication<TestApplication>();
	engine.Run();


	return 0;
}
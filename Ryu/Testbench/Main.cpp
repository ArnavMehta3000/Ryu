#include <Windows.h>
#include <Core/Log/Log.h>
#include <Core/Utils/DllLoader.h>
#include <Engine/Engine.h>
#include <Engine/Globals.h>

int WINAPI wWinMain(
	[[maybe_unused]] HINSTANCE hInstance,
	[[maybe_unused]] HINSTANCE hPrevInstance,
	[[maybe_unused]] LPWSTR pCmdLine,
	[[maybe_unused]] int nCmdShow)
{
	Ryu::DllLoader dll;
	bool dllResult = dll.LoadDLL("RyuEngine.dll");
	auto r = dll.LoadProcAddress("g_engine");
	auto createEngine = dll.GetProcAddressFunc<Ryu::InitializeEngineInstance_f>("InitializeEngineInstance");
	auto engine = dll.GetProcAddressPtr<Ryu::Engine>("g_engine");
	
	// Test code
	engine->Run();


	//RYU_TRACE("This is a trace message");
	//RYU_INFO("This is an info message");
	//RYU_WARN("This is a warning message");
	//RYU_ERROR("This is an error message");

	return 0;
}
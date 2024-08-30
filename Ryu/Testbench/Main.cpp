#include <Windows.h>
#include <Core/Log/Log.h>
#include <Core/Log/Logger.h>
#include <Core/Utils/DllLoader.h>
#include <Engine/Engine.h>
#include <Engine/Globals.h>

int WINAPI wWinMain(
	_In_     MAYBE_UNUSED HINSTANCE hInstance,
	_In_opt_ MAYBE_UNUSED HINSTANCE hPrevInstance,
	_In_     MAYBE_UNUSED LPWSTR    lpCmdLine,
	_In_     MAYBE_UNUSED int       nCmdShow)
{
	Ryu::DllLoader dll;
	bool dllResult = dll.LoadDLL("RyuEngine.dll");
	auto r = dll.LoadProcAddress("g_engine");
	auto createEngine = dll.GetProcAddressFunc<Ryu::InitializeEngineInstance_f>("InitializeEngineInstance");
	auto engine = dll.GetProcAddressPtr<Ryu::Engine>("g_engine");
	
	// Test code
	engine->Run();


	RYU_CORE_LOG_TRACE(Testbench, "This is a trace message");
	RYU_CORE_LOG_INFO(Testbench, "This is an info message");
	RYU_CORE_LOG_WARN(Testbench, "This is a warning message");
	RYU_CORE_LOG_ERROR(Testbench, "This is an error message");
	RYU_CORE_LOG_FATAL(Testbench, "This is a fatal error message");

	return 0;
}
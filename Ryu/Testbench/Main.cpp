#include <Windows.h>
#include <Core/Log/Log.h>
#include <Core/Log/Logger.h>
#include <Core/Utils/DllLoader.h>
#include <Engine/Engine.h>
#include <Engine/Globals.h>
#include <Core/Application/Window.h>

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

	Ryu::Window window;
	std::ignore = window.Create();
	window.Show();

	while (window.IsOpen())
	{
		window.PumpMessages();
	}


	return 0;
}
#include "Engine/Entry/EntryPoint.h"
#include "Testbench/TestApp.h"
#include <Window/Window.h>
#include <print>
#include <iostream>


using namespace Ryu;

RYU_USE_APP(TestApp)

RYU_MAIN()
{
	RYU_DBG_TRACK_MEM();

	App::App app(Window::WindowConfig::Get());
	std::shared_ptr<Window::Window> window = app.GetWindow();
	
	app.Run();

	//Ryu::Engine::Run();

	return 0;
}

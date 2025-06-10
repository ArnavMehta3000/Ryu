#include "Engine/Entry/EntryPoint.h"
#include "Testbench/TestApp.h"
#include <Window/Window.h>
#include <print>
#include <iostream>


/*
Target user interface
- Application (Standalone game/Editor) creates a window
- 
*/


using namespace Ryu;

struct TestInterface : public App::IApplication
{
	i32 X = 5;
};

RYU_USE_APP(TestApp)

RYU_MAIN()
{
	RYU_DBG_TRACK_MEM();

	// Create window
	std::shared_ptr<Window::Window> window = std::make_shared<Window::Window>();
	window->Create();
	window->Input.EnableRawMouseInput(true);
	window->IsDarkMode = true;

	// Create application interface
	std::shared_ptr<App::IApplication> appInterface = std::make_shared<TestInterface>();

    App::App app;
    app.Window = window;
	app.Application = appInterface;
    app.Run();

	//Ryu::Engine::Run();

	return 0;
}

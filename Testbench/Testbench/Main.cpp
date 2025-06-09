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

	std::shared_ptr<Window::Window> window = std::make_shared<Window::Window>();
	window->Input.EnableRawMouseInput(true);
	window->Create();

	const auto visitor = Window::WindowEventVisitor
	{
		[](const Window::ResizeEvent& e)
		{
			std::println("Resizing window {}x{}", e.Width, e.Height);
		},
		[](const Window::CloseEvent& e)
		{
			std::println("Closing window");
		},
	};
	

	window->AddEventListener([&visitor](const Window::WindowEvent& event)
	{
		std::visit(visitor, event);
	});

    App::App app;
    app.Window = window;
    app.Run();

	//Ryu::Engine::Run();

	return 0;
}

#include "Engine/EngineMain.h"
#include "Testbench/TestbenchApp.h"

using namespace Ryu;

RYU_MAIN()
{
	// If the game is not being built as a DLL
#if !defined(RYU_GAME_AS_DLL)
	RYU_DBG_TRACK_MEM();
	
	// Create application window
	Window::Window::Config windowConfig
	{
		.Title = "Testbench App"
	};

	auto window = std::make_shared<Window::Window>(windowConfig);
	App::App::InitWindow(*window);
	auto app = std::make_shared<TestbenchApp>(window);

#if defined(RYU_BUILD_DEBUG)  // Close on escape
	auto visitor = Window::WindowEventVisitor
	{
		[&](const Window::KeyEvent& e)
		{
			using namespace Ryu::Window;

			if (e.KeyCode == KeyCode::Escape)
			{
				app->Exit();
			}
		}
	};
	window->AddEventListener([&visitor](const Window::WindowEvent& e) { std::visit(visitor, e); });
#endif


	Ryu::Engine::Engine::Get().RunApp(app);
#endif

	return 0;
}

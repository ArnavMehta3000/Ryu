#include "Engine/Setup/Setup.h"
#include "Engine/Setup/EngineMain.h"
#include "Testbench/TestbenchApp.h"

using namespace Ryu;

#if !defined(RYU_WITH_EDITOR)
RYU_MAIN()
{
	RYU_DBG_TRACK_MEM();

	try
	{
		if (!Engine::Setup())
		{
			Engine::Shutdown();
			return -1;
		}

		Window::Window::Config windowConfig
		{
			.Title               = "Testbench",
			.CanBorderlessResize = true,
			.CanBorderlessDrag   = true,
			.Type                = Window::WindowType::Borderless
		};

		// Create window
		auto window = std::make_shared<Window::Window>(windowConfig);
		App::App::InitWindow(*window);

		// Run app
		Ryu::Engine::Engine::Get().RunApp(std::make_shared<TestbenchApp>(window));
		return 0;
	}
	catch (const AssertException& e)
	{
		RYU_LOG_FATAL("{}", e.what());

		Engine::Shutdown();
	}
}
#endif

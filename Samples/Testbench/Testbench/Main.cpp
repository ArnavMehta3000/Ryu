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

		// Create application window
		Window::Window::Config windowConfig
		{
			.Title = "Testbench App"
			.Type  = Window::WindowType::Borderless
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
					app->Quit();
				}
			}
		};
		window->AddEventListener([&visitor](const Window::WindowEvent& e) { std::visit(visitor, e); });
#endif

		auto& engine = Ryu::Engine::Engine::Get();
		engine.RunApp(app);
		return 0;
	}
	catch (const AssertException& e)
	{
		RYU_LOG_FATAL("{}", e.what());

		Engine::Shutdown();
	}
}
#endif

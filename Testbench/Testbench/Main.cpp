#include "Engine/Entry/EntryPoint.h"
#include "Engine/Engine.h"
#include "Testbench/TestbenchApp.h"

using namespace Ryu;

RYU_USE_APP(TestbenchApp)

RYU_MAIN()
{
	RYU_DBG_TRACK_MEM();

	auto app = std::make_shared<App::App>(Window::WindowConfig::Get());
	auto window = app->GetWindow();

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

	Ryu::Engine::Engine::Get().RunApp(app);

	return 0;
}

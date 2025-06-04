#include "Engine/Entry/EntryPoint.h"
#include "Testbench/TestApp.h"
#include <Window/Window.h>

RYU_USE_APP(TestApp)

RYU_MAIN()
{
	RYU_DBG_TRACK_MEM();

	Ryu::Window::Window window;
	if (window.Create())
	{
		bool isRunning = true;
		window.AddEventListener([&](const Ryu::Window::WindowEvent& event)
		{
			std::visit([&](const auto& e)
			{
				using T = std::decay_t<decltype(e)>;
				if constexpr (std::is_same_v<T, Ryu::Window::KeyEvent>)
				{
					if (e.KeyCode == VK_ESCAPE && e.IsDown == false)
					{
						isRunning = false;
					}
				}
			}, event);
		});

		while (isRunning)
		{
			window.Update();
			auto& events = Ryu::Window::Window::GetPendingEvents();
			for (const auto& e : events)
			{
				if (std::holds_alternative<Ryu::Window::CloseEvent>(e)) 
				{
					isRunning = false;
					break;
				}
			}
		}
	}

	//Ryu::Engine::Run();

	return 0;
}

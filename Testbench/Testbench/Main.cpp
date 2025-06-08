#include "Engine/Entry/EntryPoint.h"
#include "Testbench/TestApp.h"
#include <Window/Window.h>
#include <print>
#include <iostream>

RYU_USE_APP(TestApp)

RYU_MAIN()
{
	RYU_DBG_TRACK_MEM();

	Ryu::Window::Window window;
	window.Input.EnableRawMouseInput(true);

	if (window.Create())
	{
		bool isRunning = true;
		window.AddEventListener([&](const Ryu::Window::WindowEvent& event)
		{
			std::visit([&](const auto& e)
			{
				using namespace Ryu;

				using T = Window::EventType<decltype(e)>;
				if constexpr (Window::IsEventType<Window::ResizeEvent, T>())
				{
					std::println("Resize: {}x{}", e.Width, e.Height);

				}

			}, event);
		});

		// Add window event listeners (input events come through here)
        window.AddEventListener([](const Ryu::Window::WindowEvent& event) {
            std::visit([](const auto& e) {
                using T = std::decay_t<decltype(e)>;

                if constexpr (std::is_same_v<T, Ryu::Window::KeyEvent>)
                {
                    std::cout << "Key " << static_cast<int>(e.KeyCode) << " - ";

                    switch (e.State)
                    {
                    case Ryu::Window::KeyState::Down:
                        std::cout << "Pressed";
                        break;
                    case Ryu::Window::KeyState::Held:
                        std::cout << "Held (repeat)";
                        break;
                    case Ryu::Window::KeyState::Released:
                        std::cout << "Released";
                        break;
                    case Ryu::Window::KeyState::Up:
                        std::cout << "Up";
                        break;
                    }

                    if (Ryu::Window::HasModifierKey(e.Modifiers, Ryu::Window::ModifierKeys::Shift))
                        std::cout << " (with Shift)";
                    if (Ryu::Window::HasModifierKey(e.Modifiers, Ryu::Window::ModifierKeys::Control))
                        std::cout << " (with Ctrl)";

                    std::cout << std::endl;
                }
                else if constexpr (std::is_same_v<T, Ryu::Window::MouseButtonEvent>)
                {
                    std::cout << "Mouse button " << static_cast<int>(e.Button) << " - ";

                    switch (e.State)
                    {
                    case Ryu::Window::KeyState::Down:
                        std::cout << "Pressed";
                        break;
                    case Ryu::Window::KeyState::Released:
                        std::cout << "Released";
                        break;
                    default:
                        break;
                    }

                    std::cout << " at (" << e.X << ", " << e.Y << ")" << std::endl;
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
				if (std::holds_alternative<Ryu::Window::KeyEvent>(e))
				{
					if (std::get<Ryu::Window::KeyEvent>(e).KeyCode == Ryu::Window::KeyCode::Escape)
					{
						isRunning = false;
						break;
					}
				}
			}
		}
	}

	//Ryu::Engine::Run();

	return 0;
}

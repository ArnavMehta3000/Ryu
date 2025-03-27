#include "Application.h"
#include <objbase.h>

namespace Ryu::App
{
	Application::Application()
		: Elos::AppBase()
	{
		ConfigureConnections();
	}

	Application::~Application()
	{
		m_windowEventConnections.DisconnectAll();
	}

	bool Application::Init()
	{
		::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		return OnInit();
	}

	void Application::Shutdown()
	{
		OnShutdown();
	}

	void Application::Tick(const f64 dt)
	{
		OnTick(dt);
	}
	
	void Application::GetWindowCreateInfo(Elos::WindowCreateInfo& outCreateInfo)
	{
		outCreateInfo.Title = "Ryu Engine";
	}

	void Application::OnWindowClosedEvent()
	{
		if (m_window)
		{
			m_window->Close();
		}
	}
	
	void Application::ConfigureConnections()
	{
		SetUpConnection<const Elos::Event::Closed&>(m_windowEventConnections,              [this](const auto&)   { OnWindowClosedEvent();               });
		SetUpConnection<const Elos::Event::FocusLost&>(m_windowEventConnections,           [this](const auto& e) { OnWindowFocusLostEvent(e);           });
		SetUpConnection<const Elos::Event::FocusGained&>(m_windowEventConnections,         [this](const auto& e) { OnWindowFocusGainedEvent(e);         });
		SetUpConnection<const Elos::Event::MouseEntered&>(m_windowEventConnections,        [this](const auto& e) { OnWindowMouseEnteredEvent(e);        });
		SetUpConnection<const Elos::Event::MouseLeft&>(m_windowEventConnections,           [this](const auto& e) { OnWindowMouseLeftEvent(e);           });
		SetUpConnection<const Elos::Event::Resized&>(m_windowEventConnections,             [this](const auto& e) { OnWindowResizedEvent(e);             });
		SetUpConnection<const Elos::Event::TextInput&>(m_windowEventConnections,           [this](const auto& e) { OnWindowTextInputEvent(e);           });
		SetUpConnection<const Elos::Event::KeyPressed&>(m_windowEventConnections,          [this](const auto& e) { OnWindowKeyPressedEvent(e);          });
		SetUpConnection<const Elos::Event::KeyReleased&>(m_windowEventConnections,         [this](const auto& e) { OnWindowKeyReleasedEvent(e);         });
		SetUpConnection<const Elos::Event::MouseWheelScrolled&>(m_windowEventConnections,  [this](const auto& e) { OnWindowMouseWheelScrolledEvent(e);  });
		SetUpConnection<const Elos::Event::MouseButtonPressed&>(m_windowEventConnections,  [this](const auto& e) { OnWindowMouseButtonPressedEvent(e);  });
		SetUpConnection<const Elos::Event::MouseButtonReleased&>(m_windowEventConnections, [this](const auto& e) { OnWindowMouseButtonReleasedEvent(e); });
		SetUpConnection<const Elos::Event::MouseMoved&>(m_windowEventConnections,          [this](const auto& e) { OnWindowMouseMovedEvent(e);          });
		SetUpConnection<const Elos::Event::MouseMovedRaw&>(m_windowEventConnections,       [this](const auto& e) { OnWindowMouseMovedRawEvent(e);       });
	}
}

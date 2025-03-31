#include "Application.h"
#include "App/AppConfig.h"
#include "Profiling/Profiling.h"
#include <Elos/Window/Utils/WindowExtensions.h>

namespace Ryu::App
{
	Application::Application() : Elos::AppBase()
	{
		RYU_PROFILE_SCOPE();
		ConfigureConnections();
	}

	Application::~Application()
	{
		RYU_PROFILE_SCOPE();
		m_windowEventConnections.DisconnectAll();
	}

	bool Application::Init()
	{
		RYU_PROFILE_SCOPE();
		::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		
		AppBase::InitializeWindow();
		Elos::WindowExtensions::EnableDarkMode(m_window->GetHandle(), true);
		m_window->RequestFocus();

		return OnInit();
	}

	void Application::Shutdown()
	{
		RYU_PROFILE_SCOPE();
		OnShutdown();
	}

	void Application::Tick(const f64 dt)
	{
		RYU_PROFILE_SCOPE();
		OnTick(dt);
	}

	bool Application::OnInit()
	{
		RYU_PROFILE_SCOPE();

		const AppConfig& config = AppConfig::Get();
		const auto& minSize = config.WindowMinSize.Get();
		m_window->SetMinimumSize({ minSize[0], minSize[1] });

		return true;
	}

	void Application::OnShutdown()
	{
		RYU_PROFILE_SCOPE();
	}
	
	void Application::OnTick(MAYBE_UNUSED f64 dt)
	{
		TracyPlot("Delta Time", dt);
	}

	void Application::GetWindowCreateInfo(Elos::WindowCreateInfo& outCreateInfo)
	{
		RYU_PROFILE_SCOPE();
		const AppConfig& config = AppConfig::Get();
		const auto& size        = config.WindowSize.Get();
		const auto& pos         = config.WindowPos.Get();

		outCreateInfo.Title    = config.WindowTitle;
		outCreateInfo.Size     = { size[0], size[1] };
		outCreateInfo.Position = { pos[0], pos[1] };
	}

	void Application::OnWindowClosedEvent()
	{
		if (m_window)
		{
			m_window->Close();
		}
	}

	void Application::OnWindowKeyPressedEvent(const Elos::Event::KeyPressed& e)
	{
#if defined(RYU_BUILD_DEBUG)
		if (e.Key == Elos::KeyCode::Escape)
		{
			m_window->Close();
		}
#endif
	}

	void Application::ConfigureConnections()
	{
		RYU_PROFILE_SCOPE();
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

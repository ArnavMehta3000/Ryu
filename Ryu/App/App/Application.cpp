#include "Application.h"
#include "App/AppConfig.h"
#include "Logger/Sinks/DebugSink.h"
#include "Logger/Sinks/ConsoleSink.h"
#include "Logger/Sinks/FileSink.h"
#include "Logger/Assert.h"
#include "Utils/MessageBox.h"
#include "Profiling/Profiling.h"
#include <Elos/Window/Utils/WindowExtensions.h>

namespace Ryu::App
{
	namespace Internal
	{
		static std::string_view g_appWindowNotSetError = "Application window not set";
		static std::string_view g_appWindowNotOpenError = "Application window not open";
	}

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

	void Application::Tick(const Utils::TimeInfo& timeInfo)
	{
		RYU_PROFILE_SCOPE();
		OnTick(timeInfo);
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
			RYU_PROFILE_BOOKMARK("On window closed event");
			m_window->Close();
		}
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

	void App::Run()
	{
		RYU_ASSERT(m_window, Internal::g_appWindowNotSetError);
		RYU_ASSERT(m_window->IsOpen, Internal::g_appWindowNotOpenError);

		while (m_window && m_window->IsOpen)
		{
			m_window->Update();
			m_window->ClearPendingEvents();
		}
	}
}

void Ryu::App::Internal::SetUpDefaultLogger()
{
	RYU_LOG_DECLARE_CATEGORY(Ryu);
	using namespace Ryu::Logging;

	Logger& logger = Logger::Get();
	const AppConfig& config = AppConfig::Get();

	logger.SetOnFatalCallback([](Logging::LogLevel level, const Logging::LogMessage& message)
	{
		Utils::MessageBoxDesc desc;
		desc.Title        = EnumToString(level);
		desc.Title       += " Error";
		desc.Text         = message.Message;
		desc.Flags.Button = Utils::MessagBoxButton::Ok;
		desc.Flags.Icon   = Utils::MessageBoxIcon::Error;

		Utils::ShowMessageBox(desc);
		std::abort();
	});

	// Log to output window only when debugger is attached
	if (Common::Globals::IsDebuggerAttached() || config.ForceLogToOutput)
	{
		logger.AddSink(std::make_unique<Logging::DebugSink>());
	}
	
	if (config.EnableLogToConsole)
	{
		logger.AddSink(std::make_unique<Logging::ConsoleSink>());
	}

	if (config.EnableLogToFile)
	{
		logger.AddSink(std::make_unique<Logging::FileSink>(config.LogFilePath.Get()));
		RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(Ryu), "Application log file opened: {}", config.LogFilePath.Get());
	}

	RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(Ryu), "Logging initialized");
}

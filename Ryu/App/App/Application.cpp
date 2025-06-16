#include "Application.h"
#include "App/AppConfig.h"
#include "Logger/Sinks/DebugSink.h"
#include "Logger/Sinks/ConsoleSink.h"
#include "Logger/Sinks/FileSink.h"
#include "Logger/Assert.h"
#include "Utils/MessageBox.h"
#include "Profiling/Profiling.h"
#include "App/Utils/PathManager.h"
#include <Elos/Window/Utils/WindowExtensions.h>

namespace Ryu::App
{
	Application::Application() : Elos::AppBase()
	{
		RYU_PROFILE_SCOPE();
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

	App::App(const Window::Window::Config& config)
	{
		RYU_PROFILE_SCOPE();

		m_window = std::make_shared<Window::Window>(config);
		m_window->Create();
		
		m_window->Input.EnableRawMouseInput(true);
		m_window->IsDarkMode = true;
	}

	void App::Run()
	{
		RYU_ASSERT(m_window, "Application window not set");
		RYU_ASSERT(m_window->IsOpen, "Application window not open");

		try
		{
			PreInit();
			RunInternal();
			PostShutdown();
		}
		catch (const Exception& e)
		{
			using namespace Ryu::Logging;
			// Custom version of the RYU_LOG_FATAL macro that includes a stack our assertion exception
			Logger::Get().Log(
				AssertLog,
				LogLevel::Fatal,
				LogMessage
				{
					.Message = e.what(),
					.Stacktrace = e.GetTrace()
				});
		}
		catch (const std::exception& e)
		{
			RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(App), "Unhandled exception: {}", e.what());
		}
	}

	void App::PreInit()
	{
		RYU_PROFILE_SCOPE();

		// Init path manager to get paths
		auto& pathManager = PathManager::Get();
		pathManager.Init();

		// Setup config
		Config::ConfigManager::Get().Initialize((pathManager.GetProjectDir() / "Config").string());
	
		// Setup logger
		SetupDefaultLogger();
	}

	void App::RunInternal()
	{
		// Update window events
		while (m_window && m_window->IsOpen)
		{
			m_window->Update();
			m_window->ClearPendingEvents();
		}
	}
	
	void App::PostShutdown()
	{
		RYU_PROFILE_SCOPE();
		Config::ConfigManager::Get().SaveAll();
	}

	void App::SetupDefaultLogger()
	{
		RYU_PROFILE_SCOPE();

		using namespace Ryu::Logging;
		using namespace Ryu::Utils;
		using namespace Ryu::Common;

		Logger& logger = Logger::Get();
		const AppConfig& config = AppConfig::Get();

		logger.SetOnFatalCallback([](LogLevel level, const LogMessage& message)
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
		if (Globals::IsDebuggerAttached() || config.ForceLogToOutput)
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
			RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(App), "Application log file opened: {}", config.LogFilePath.Get());
		}

		RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(App), "Logging initialized");
	}
}

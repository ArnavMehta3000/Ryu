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
	App::App(const Window::Window::Config& config)
	{
		RYU_PROFILE_SCOPE();

		m_window = std::make_shared<Window::Window>(config);
		m_window->Create();
		
		m_window->Input.EnableRawMouseInput(true);
		m_window->IsDarkMode = true;
	}

	void App::Exit()
	{
		m_isRunning = false;
		
		// Send a close message to the window
		if (m_window && m_window->IsOpen)
		{
			::SendMessage(m_window->GetHandle(), WM_CLOSE, 0, 0);
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

	void App::ProcessWindowEvents()
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

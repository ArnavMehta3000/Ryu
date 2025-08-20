#include "Engine/Setup/Setup.h"
#include "Common/Assert.h"
#include "Globals/Globals.h"
#include "Config/CmdLine.h"
#include "Logging/Logger.h"
#include "Utils/MessageBox.h"
#include <filesystem>

namespace Ryu::Engine
{
	namespace fs = std::filesystem;

#pragma region Logging CVars
	static Config::CVar<i32> s_logLevel(
		"Log.Level",
		static_cast<i32>(Logging::COMPILE_TIME_LOG_LEVEL),
		"0 = Trace | 1 = Debug | 2 = Info | 3 = Warn | 4 = Error");

	static Config::CVar<bool> s_logConsole(
		"Log.ToConsole",
		Globals::g_isDebug,  // Console log to be enabled by default in debug build
		"Output log to the console");

	static Config::CVar<bool> s_logDebug(
		"Log.DebugOutput",
		Globals::g_isDebug,  // OutputDebugString log to be enabled by default in debug build
		"Output log to debug output");

	static Config::CVar<bool> s_logFile(
		"Log.ToFile",
		false,
		"Output log to a file");

	static Config::CVar<std::string> s_logFilePath(
		"Log.FilePath",
		"Log/RyuLog.txt",
		"File path for the log file");

	static Config::CVar<bool> s_logHasTimestamp(
		"Log.HasTimestamp",
		true,
		"Timestamp log entries");

	static Config::CVar<bool> s_logHasThreadID(
		"Log.HasThreadID",
		false,
		"Include thread ID in log entries");

	static Config::CVar<std::string> s_logPattern(
		"Log.Pattern",
		std::string(),
		"Use custom log pattern");

	static Config::CVar<bool> s_logAsync(
		"Log.Async",
		false,
		"Use async logging");

	static Config::CVar<i32> s_logAsyncQueueSize(
		"Log.AsyncQueueSize",
		8192,
		"Queue size for async logging. Needs 'Log.Async' to be enabled");
#pragma endregion

	bool Setup()
	{
		if (!Config::CmdLine::Get().ParseCommandLine())
		{
			return false;
		}

		// Setup logging
		{
			// Clamp the level to the user set and the max we can have at compile time
			i32 level = s_logLevel.Get();
			level = std::clamp(level, 0, static_cast<i32>(Logging::COMPILE_TIME_LOG_LEVEL));

			Logging::LoggingConfig config{};

			config.RuntimeLogLevel = static_cast<Logging::LogLevel>(level);
			
			config.Sinks.Console                = s_logConsole;
			config.Sinks.File                   = s_logFile;
			config.Sinks.Debug                  = s_logDebug;
			config.Sinks.LogFilePath            = s_logFilePath;

			config.Pattern.IncludeTimestamp     = s_logHasTimestamp;
			config.Pattern.IncludeThreadId      = s_logHasThreadID;
			
			if (!s_logPattern.Get().empty())
			{
				config.Pattern.CustomPattern = s_logPattern;
			}

			config.AsyncLogging = s_logAsync;
			config.FlushOnError = true;  // This is not configurable by the user
			config.AsyncQueueSize = s_logAsyncQueueSize;

			// Configure the logger
			if (auto* logger = Logging::Internal::GetLoggerInstance())
			{
				logger->Configure(config);
				
				logger->SetOnFatalCallback([](Logging::LogLevel, const std::string& msg)
				{
					Utils::MessageBoxDesc desc;
					desc.Title        = "Fatal Error";
					desc.Text         = msg;
					desc.Flags.Button = Utils::MessagBoxButton::Ok;
					desc.Flags.Icon   = Utils::MessageBoxIcon::Error;

					Utils::ShowMessageBox(desc);
					
					throw Ryu::AssertException("FATAL ERROR", msg);
				});

				// Integrate logging with assertion
				Ryu::AssertManager::SetAssertHandler([](const AssertException& exception)
				{
					RYU_LOG_ERROR("Assertion failed: {}", exception.Message());

					const auto& trace = exception.GetTrace();
					if (!trace.empty())
					{
						std::string stackTraceStr;
						for (const auto& entry : trace)
						{
							stackTraceStr += fmt::format("  {} ({}:{})\n",
								entry.description(),
								entry.source_file(),
								entry.source_line());
						}
						RYU_LOG_ERROR("Stack trace:\n{}", stackTraceStr);
					}

					// Still throw the exception to maintain existing behavior
					// This should be caught by the try-catch in Engine::RunApp
					throw exception;
				});
			}
		}

		return true;
	}

	void Shutdown()
	{
		Config::CmdLine::Get().SaveFileConfig(fs::current_path() / "RyuConfig.toml");

		if (auto* logger = Logging::Internal::GetLoggerInstance())
		{
			logger->Flush();
		}
	}
}

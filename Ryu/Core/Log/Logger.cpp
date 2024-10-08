#include "Logger.h"
#include <CoreVersion.h>
#include <Core/Log/Log.h>
#include <spdlog/sinks/rotating_file_sink.h>
#if RYU_LOG_CONSOLE
#include <spdlog/sinks/stdout_color_sinks.h>
#endif
#if RYU_BUILD_DEBUG
#include <spdlog/sinks/msvc_sink.h>
#endif

namespace Ryu
{
	void Logger::Initialize(const std::string& name)
	{
		std::vector<spdlog::sink_ptr> sinks;
		const std::string pattern = "%^[%H:%M:%S.%e] [%n] [%l] %v%$";

		const std::string logFileName = "RyuLog.txt";
		const std::size_t maxFileSize = 1048576 * 5; // 5 MB
		const std::size_t maxFiles = 3; // Keep 3 rotated files
		sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFileName, maxFileSize, maxFiles));
		
#if RYU_LOG_CONSOLE
		// Create a console window manually
		if (!::GetStdHandle(STD_OUTPUT_HANDLE))
		{
			AllocConsole();

			// Redirect standard input/output to the console
			FILE* fp;
			freopen_s(&fp, "CONOUT$", "w", stdout);  // Redirect stdout to the console
			freopen_s(&fp, "CONOUT$", "w", stderr);  // Redirect stderr to the console
			freopen_s(&fp, "CONIN$", "r", stdin);    // Redirect stdin to the console
		}
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#endif

#if RYU_BUILD_DEBUG
		sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif

		auto logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
		logger->set_pattern(pattern);

		logger->set_level(spdlog::level::trace);
		logger->flush_on(spdlog::level::trace);
		spdlog::register_logger(logger);

#if RYU_LOG_TRACE_ENABLED
		logger->trace("Registered \'{}\' logger", name);
#endif
	}

	void Logger::Shutdown()
	{
		spdlog::shutdown();
	}
}

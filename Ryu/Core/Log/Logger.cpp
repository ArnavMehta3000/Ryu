#include "Logger.h"
#include <Core/Log/Log.h>
#include <spdlog/sinks/basic_file_sink.h>
#if RYU_LOG_CONSOLE
#include <spdlog/sinks/stdout_color_sinks.h>
#endif
#if RYU_BUILD_DEBUG
#include <spdlog/sinks/msvc_sink.h>
#endif

namespace Ryu
{
	void Logger::Initialize()
	{
		std::vector<spdlog::sink_ptr> sinks;
		const std::string pattern = "%^[%H:%M:%S.%e] [%l] %v%$";

		sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("RyuLog.txt"));
		
#if RYU_LOG_CONSOLE
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#endif

#if RYU_BUILD_DEBUG
		sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif

		auto logger = std::make_shared<spdlog::logger>(GetDefaultLoggerName(), sinks.begin(), sinks.end());
		logger->set_pattern(pattern);

		logger->set_level(spdlog::level::trace);
		logger->flush_on(spdlog::level::trace);

		spdlog::register_logger(logger);
	}

	void Logger::Shutdown()
	{
		spdlog::shutdown();
	}
}

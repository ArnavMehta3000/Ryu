#pragma once
#include "Core/Logging/LogLevel.h"
#include <string>

namespace Ryu::Logging
{
	struct LoggingConfig
	{
		LogLevel RuntimeLogLevel = LogLevel::Info;

        struct SinkConfig
        {
            bool Console = true;
            bool Debug = true;
            bool File = true;
            std::string LogFilePath = "Logs/RyuLog.txt";
        } Sinks;

        struct PatternConfig
        {
            bool IncludeTimestamp = true;
            bool IncludeThreadId  = false;
            std::string CustomPattern; // If set, overrides other pattern settings
        } Pattern;

        bool AsyncLogging  = false;
        bool FlushOnError  = true;
        u64 AsyncQueueSize = 8192;

        // Generate spdlog pattern string based on configuration
        std::string GetSpdlogPattern() const;
	};

    // I cannot declare CVars here it as it will cause a circular dependency with RyuConfig module
    // So the engine will process cmdline args and pass a config to the logging system
    // See Engine/Setup.cpp
}

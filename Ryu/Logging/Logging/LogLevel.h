#pragma once
#include "Common/StandardTypes.h"
#include <spdlog/spdlog.h>

namespace Ryu::Logging
{
	enum class LogLevel : i32
	{
        Trace = static_cast<i32>(spdlog::level::trace),
        Debug = static_cast<i32>(spdlog::level::debug),
        Info  = static_cast<i32>(spdlog::level::info),
        Warn  = static_cast<i32>(spdlog::level::warn),
        Error = static_cast<i32>(spdlog::level::err),
        Fatal = static_cast<i32>(spdlog::level::critical)
	};

    // Compile-time log level configuration
#if defined(RYU_LOG_LEVEL_TRACE)
    constexpr LogLevel COMPILE_TIME_LOG_LEVEL = LogLevel::Trace;
#elif defined(RYU_LOG_LEVEL_DEBUG)
    constexpr LogLevel COMPILE_TIME_LOG_LEVEL = LogLevel::Debug;
#elif defined(RYU_LOG_LEVEL_INFO)
    constexpr LogLevel COMPILE_TIME_LOG_LEVEL = LogLevel::Info;
#elif defined(RYU_LOG_LEVEL_WARN)
    constexpr LogLevel COMPILE_TIME_LOG_LEVEL = LogLevel::Warn;
#elif defined(RYU_LOG_LEVEL_ERROR)
    constexpr LogLevel COMPILE_TIME_LOG_LEVEL = LogLevel::Error;
#elif defined(RYU_LOG_LEVEL_FATAL)
    constexpr LogLevel COMPILE_TIME_LOG_LEVEL = LogLevel::Fatal;
#else
    #if defined(RYU_BUILD_DEBUG)
        constexpr LogLevel COMPILE_TIME_LOG_LEVEL = LogLevel::Debug;
    #else
        constexpr LogLevel COMPILE_TIME_LOG_LEVEL = LogLevel::Info;
    #endif
#endif

    namespace Internal
    {
        inline spdlog::level::level_enum ToSpdlogLevel(LogLevel level)
        {
            return static_cast<spdlog::level::level_enum>(level);
        }

        inline LogLevel FromSpdlogLevel(spdlog::level::level_enum level)
        {
            return static_cast<LogLevel>(level);
        }
    }
}

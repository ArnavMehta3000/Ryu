#pragma once
#include <CoreLogDefines.h>
#include <CoreVersion.h>
#include <Core/Log/StackTrace.h>
#include <spdlog/spdlog.h>

namespace Ryu
{
	constexpr const char* GetDefaultLoggerName() { return "RyuLogger"; }
}

#ifndef RYU_FATAL
// Logs a fatal error message
#define RYU_FATAL(...) spdlog::get(::Ryu::GetDefaultLoggerName())->critical(__VA_ARGS__);
#endif

#ifndef RYU_ERROR
// Logs an error message
#define RYU_ERROR(...) spdlog::get(::Ryu::GetDefaultLoggerName())->error(__VA_ARGS__);
#endif

#if RYU_LOG_WARN_ENABLED == 1
// Logs a warning message
#define RYU_WARN(...) spdlog::get(::Ryu::GetDefaultLoggerName())->warn(__VA_ARGS__);
#else
#define RYU_WARN(...)
#endif

#if RYU_LOG_INFO_ENABLED == 1
// Logs an info message
#define RYU_INFO(...) spdlog::get(::Ryu::GetDefaultLoggerName())->info(__VA_ARGS__);
#else
#define RYU_INFO(...)
#endif

#if RYU_LOG_DEBUG_ENABLED == 1
// Logs a debug message
#define RYU_DEBUG(...) spdlog::get(::Ryu::GetDefaultLoggerName())->debug(__VA_ARGS__);
#else
#define RYU_DEBUG(...)
#endif

#if RYU_LOG_TRACE_ENABLED == 1
// Logs a trace message
#define RYU_TRACE(...) spdlog::get(::Ryu::GetDefaultLoggerName())->trace(__VA_ARGS__);
#else
#define RYU_TRACE(...)
#endif

#if RYU_ASSERTIONS_ENABLED
#define RYU_ASSERT(cond, message)                                                      \
{                                                                                      \
	if (!(cond))                                                                       \
	{                                                                                  \
		::Ryu::StackTrace st = ::Ryu::StackTrace();                                    \
		RYU_FATAL("Assertiion failed: {}\nMessage:{}\n{}", #cond, message, st.Print());\
		__debugbreak();                                                                \
	}                                                                                  \
}
#else
#define RYU_ASSERT(cond, message)
#endif
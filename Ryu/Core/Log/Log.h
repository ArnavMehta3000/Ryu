#pragma once
#include <CoreLogDefines.h>
#include <Core/Log/StackTrace.h>
#include <spdlog/spdlog.h>

#define RYU_CORE_GET_LOGGER(name) \
	(spdlog::get(name) ? spdlog::get(name) : (Ryu::Logger::Initialize(name), spdlog::get(name)))

#ifndef RYU_CORE_LOG_FATAL
// Logs a fatal error message
#define RYU_CORE_LOG_FATAL(name, ...) RYU_CORE_GET_LOGGER(#name)->critical(__VA_ARGS__); __debugbreak()
#endif

#ifndef RYU_CORE_LOG_ERROR
// Logs an error message
#define RYU_CORE_LOG_ERROR(name, ...) RYU_CORE_GET_LOGGER(#name)->error(__VA_ARGS__)
#endif

#if RYU_LOG_WARN_ENABLED == 1
// Logs a warning message
#define RYU_CORE_LOG_WARN(name, ...) RYU_CORE_GET_LOGGER(#name)->warn(__VA_ARGS__)
#else
#define RYU_CORE_LOG_WARN(name, ...)
#endif

#if RYU_LOG_INFO_ENABLED == 1
// Logs an info message
#define RYU_CORE_LOG_INFO(name, ...) RYU_CORE_GET_LOGGER(#name)->info(__VA_ARGS__)
#else
#define RYU_CORE_LOG_INFO(name, ...)
#endif

#if RYU_LOG_DEBUG_ENABLED == 1
// Logs a debug message
#define RYU_CORE_LOG_DEBUG(name, ...) RYU_CORE_GET_LOGGER(#name)->debug(__VA_ARGS__)
#else
#define RYU_CORE_LOG_DEBUG(name, ...)
#endif

#if RYU_LOG_TRACE_ENABLED == 1
// Logs a trace message
#define RYU_CORE_LOG_TRACE(name, ...) RYU_CORE_GET_LOGGER(#name)->trace(__VA_ARGS__)
#else
#define RYU_CORE_LOG_TRACE(name, ...)
#endif

#define RYU_CORE_LOG_SHUTDOWN() ::Ryu::Logger::Shutdown()
#define RYU_CORE_LOG_INITIALIZE(name) ::Ryu::Logger::Initialize(name)

#if RYU_ASSERTIONS_ENABLED
#define RYU_CORE_ASSERT(name, cond, message)                                                           \
{                                                                                                      \
	if (!(cond))                                                                                       \
	{                                                                                                  \
		::Ryu::StackTrace st = ::Ryu::StackTrace();                                                    \
		RYU_CORE_LOG_FATAL(name, "Assertion failed: {}\nMessage: {}\n{}", #cond, message, st.Print()); \
		__debugbreak();                                                                                \
	}                                                                                                  \
}
#else
#define RYU_CORE_ASSERT(name, cond, message)
#endif
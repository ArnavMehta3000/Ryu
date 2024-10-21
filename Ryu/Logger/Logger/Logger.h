#pragma once
#include "Common/ObjectMacros.h"
#include "Logger/Sinks/ILogSink.h"
#include "Logger/LogCategory.h"
#include "Utils/Singleton.h"
#include <memory>
#include <mutex>
#include <vector>
#include <functional>
#include <stacktrace>

namespace Ryu::Logging
{
	class Logger
	{
		RYU_DECLARE_SINGLETON(Logger, false);
	public:
		using OnLogCallback = std::function<void(LogLevel, const LogMessage&)>;

	public:
		Logger() = default;

		void AddSink(std::unique_ptr<ILogSink> sink);
		void Log(const LogCategory& category, LogLevel level, const LogMessage& message) const;

		inline void SetOnFatalCallback(OnLogCallback callback) { m_onFatalCallback = callback; }

	private:
		std::vector<std::unique_ptr<ILogSink>> m_sinks;
		mutable std::mutex                     m_mutex;  // Marked mutable to allow std::lock_guard
		OnLogCallback                          m_onFatalCallback = [](MAYBE_UNUSED LogLevel, MAYBE_UNUSED const LogMessage&) { std::abort(); };
	};
}

#define LOG_TRACE(category, message, ...) ::Ryu::Logging::Logger::Get()->Log(category, ::Ryu::Logging::LogLevel::Trace, ::Ryu::Logging::LogMessage{ std::format(message, __VA_ARGS__), std::stacktrace::current() })
#define LOG_DEBUG(category, message, ...) ::Ryu::Logging::Logger::Get()->Log(category, ::Ryu::Logging::LogLevel::Debug, ::Ryu::Logging::LogMessage{ std::format(message, __VA_ARGS__), std::stacktrace::current() })
#define LOG_INFO(category, message, ...)  ::Ryu::Logging::Logger::Get()->Log(category, ::Ryu::Logging::LogLevel::Info, ::Ryu::Logging::LogMessage{ std::format(message, __VA_ARGS__), std::stacktrace::current() })
#define LOG_WARN(category, message, ...)  ::Ryu::Logging::Logger::Get()->Log(category, ::Ryu::Logging::LogLevel::Warn, ::Ryu::Logging::LogMessage{ std::format(message, __VA_ARGS__), std::stacktrace::current() })
#define LOG_ERROR(category, message, ...) ::Ryu::Logging::Logger::Get()->Log(category, ::Ryu::Logging::LogLevel::Error, ::Ryu::Logging::LogMessage{ std::format(message, __VA_ARGS__), std::stacktrace::current() })
#define LOG_FATAL(category, message, ...) ::Ryu::Logging::Logger::Get()->Log(category, ::Ryu::Logging::LogLevel::Fatal, ::Ryu::Logging::LogMessage{ std::format(message, __VA_ARGS__), std::stacktrace::current() })

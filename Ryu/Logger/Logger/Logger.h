#pragma once
#include "Common/ObjectMacros.h"
#include "Logger/Sinks/ILogSink.h"
#include "Logger/LogCategory.h"
#include <memory>
#include <mutex>
#include <vector>
#include <functional>

namespace Ryu::Logging
{
	class Logger
	{
	public:
		using OnLogCallback = std::function<void(LogLevel, const std::string&)>;

	public:
		static Logger& Get();

		void AddSink(std::unique_ptr<ILogSink> sink);
		void Log(const LogCategory& category, LogLevel level, const std::string& message) const;

		inline void SetOnFatalCallback(OnLogCallback callback) { m_onFatalCallback = callback; }

	private:
		Logger() = default;

	private:
		std::vector<std::unique_ptr<ILogSink>> m_sinks;
		mutable std::mutex                     m_mutex;  // Marked mutable to allow std::lock_guard
		OnLogCallback                          m_onFatalCallback = [](MAYBE_UNUSED LogLevel, MAYBE_UNUSED const std::string&) { std::abort(); };
	};
}

#define LOG_TRACE(category, message, ...) ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Trace, std::format(message, __VA_ARGS__))
#define LOG_DEBUG(category, message, ...) ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Debug, std::format(message, __VA_ARGS__))
#define LOG_INFO(category, message, ...)  ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Info, std::format(message, __VA_ARGS__))
#define LOG_WARN(category, message, ...)  ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Warn, std::format(message, __VA_ARGS__))
#define LOG_ERROR(category, message, ...) ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Error, std::format(message, __VA_ARGS__))
#define LOG_FATAL(category, message, ...) ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Fatal, std::format(message, __VA_ARGS__))

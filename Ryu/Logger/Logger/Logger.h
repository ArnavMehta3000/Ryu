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
#include <format>

namespace Ryu::Logging
{
	/**
	 * @brief Main logging class
	 * @details This is a singleton class which can be accessed via the Logger::Get() function
	 */
	class Logger : public Utils::Singleton<Logger>
	{
		RYU_SINGLETON_DECLARE(Logger);
	public:
		/**
		 * @brief Callback type for when a log message is dispatched
		 */
		using OnLogCallback = std::function<void(LogLevel, const LogMessage&)>;

		/**
		 * @brief Add a new output sink to the logger
		 * @param sink The sink to add
		 */
		RYU_API void AddSink(std::unique_ptr<ILogSink> sink);

		/**
		 * @brief Log a message
		 * @param category The `LogCategory` to use
		 * @param level The `LogLevel` to use
		 * @param message The `LogMessage` to dispatch
		 */
		RYU_API void Log(const LogCategory& category, LogLevel level, const LogMessage& message) const;

		/**
		 * @brief Set a custom callback for fatal errors
		 * @param callback The callback
		 */
		inline RYU_API void SetOnFatalCallback(OnLogCallback callback) { m_onFatalCallback = callback; }

	private:
		Logger() = default;

	private:
		std::vector<std::unique_ptr<ILogSink>> m_sinks;
		mutable std::mutex                     m_mutex;  // Marked mutable to allow std::lock_guard
		OnLogCallback                          m_onFatalCallback = [](MAYBE_UNUSED LogLevel, MAYBE_UNUSED const LogMessage&) { std::abort(); };
	};
}

#define LOG_TRACE(category, ...) ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Trace, ::Ryu::Logging::LogMessage{ std::format(__VA_ARGS__), std::stacktrace::current() })
#define LOG_DEBUG(category, ...) ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Debug, ::Ryu::Logging::LogMessage{ std::format(__VA_ARGS__), std::stacktrace::current() })
#define LOG_INFO(category, ...)  ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Info, ::Ryu::Logging::LogMessage{ std::format(__VA_ARGS__), std::stacktrace::current() })
#define LOG_WARN(category,  ...)  ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Warn, ::Ryu::Logging::LogMessage{ std::format(__VA_ARGS__), std::stacktrace::current() })
#define LOG_ERROR(category, ...) ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Error, ::Ryu::Logging::LogMessage{ std::format(__VA_ARGS__), std::stacktrace::current() })
#define LOG_FATAL(category, ...) ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Fatal, ::Ryu::Logging::LogMessage{ std::format(__VA_ARGS__), std::stacktrace::current() })

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

	struct FormattedLogMessage
	{
		std::string Timestamp;
		std::string Level;
		std::string Category;
		std::string Message;
		std::string Stacktrace;
	};

	static FormattedLogMessage DefaultFormatter(const LogCategory& category, LogLevel level, const LogMessage& message, bool captureStackTrace);
	static std::string DefaultFormatter(LogLevel level, const FormattedLogMessage& message);
}


#if defined (RYU_LOG_ENABLED_TRACE)
// Log verbose information
#define RYU_LOG_TRACE(category, ...) ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Trace, ::Ryu::Logging::LogMessage{ std::format(__VA_ARGS__), std::stacktrace::current() })
#else
#define RYU_LOG_TRACE(category, ...) void(0)
#endif

#if defined (RYU_LOG_ENABLED_DEBUG)
// Log debug information
#define RYU_LOG_DEBUG(category, ...) ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Debug, ::Ryu::Logging::LogMessage{ std::format(__VA_ARGS__), std::stacktrace::current() })
#else
#define RYU_LOG_DEBUG(category, ...) void(0)
#endif

#if defined (RYU_LOG_ENABLED_INFO)
// Log information
#define RYU_LOG_INFO(category, ...)  ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Info, ::Ryu::Logging::LogMessage{ std::format(__VA_ARGS__), std::stacktrace::current() })
#else
#define RYU_LOG_INFO(category, ...) void(0)
#endif

#if defined (RYU_LOG_ENABLED_WARN)
// Log warnings
#define RYU_LOG_WARN(category,  ...)  ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Warn, ::Ryu::Logging::LogMessage{ std::format(__VA_ARGS__), std::stacktrace::current() })
#else
#define RYU_LOG_WARN(category,  ...) void(0)
#endif

// Log an error
 #define RYU_LOG_ERROR(category, ...) ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Error, ::Ryu::Logging::LogMessage{ std::format(__VA_ARGS__), std::stacktrace::current() })

// Log a fatal error (abort)
#define RYU_LOG_FATAL(category, ...) ::Ryu::Logging::Logger::Get().Log(category, ::Ryu::Logging::LogLevel::Fatal, ::Ryu::Logging::LogMessage{ std::format(__VA_ARGS__), std::stacktrace::current() })

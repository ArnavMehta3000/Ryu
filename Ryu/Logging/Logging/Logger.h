#pragma once
#include "Common/API.h"
#include "Common/ObjectMacros.h"
#include "Logging/LoggingConfig.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/fmt/fmt.h>
#include <memory>
#include <functional>
#include <unordered_map>

#define RYU_LOG_DECLARE_CATEGORY(CategoryName) static inline constexpr ::Ryu::Logging::LogCategory RYU_CONCAT(Log, CategoryName){ #CategoryName }

namespace Ryu::Logging
{
	struct LogCategory
	{
		std::string_view Name;

		explicit constexpr LogCategory(std::string_view name) : Name(name) {}
	};

	class Logger
	{
	public:
		using OnFatalCallback = std::function<void(LogLevel, const std::string&)>;

		RYU_API Logger();
		RYU_API ~Logger();

		RYU_API void Configure(const LoggingConfig& config);
		RYU_API void SetRuntimeLogLevel(LogLevel level);
		inline LogLevel GetRuntimeLogLevel() const { return m_config.RuntimeLogLevel; }

		RYU_API void EnableSink(const std::string& sinkName, bool enable = true);
		RYU_API void DisableSink(const std::string& sinkName);
		RYU_API bool IsSinkEnabled(const std::string& sinkName) const;
		inline void SetOnFatalCallback(OnFatalCallback callback) { m_onFatalCallback = std::move(callback); }

		RYU_API std::shared_ptr<spdlog::logger> GetCategoryLogger(const LogCategory& category);

		RYU_API void Flush();

		template<typename... Args>
		void Log(const LogCategory& category, LogLevel level, fmt::format_string<Args...> format, Args&&... args);

		template<typename... Args>
		void Trace(const LogCategory& category, fmt::format_string<Args...> format, Args&&... args);

		template<typename... Args>
		void Debug(const LogCategory& category, fmt::format_string<Args...> format, Args&&... args);

		template<typename... Args>
		void Info(const LogCategory& category, fmt::format_string<Args...> format, Args&&... args);

		template<typename... Args>
		void Warn(const LogCategory& category, fmt::format_string<Args...> format, Args&&... args);

		template<typename... Args>
		void Error(const LogCategory& category, fmt::format_string<Args...> format, Args&&... args);

		template<typename... Args>
		void Fatal(const LogCategory& category, fmt::format_string<Args...> format, Args&&... args);

	private:
		void CreateSinks(const LoggingConfig& config);
		void UpdateLoggers();
		std::string FormatCategoryName(const LogCategory& category) const;

	private:
		LoggingConfig                                                    m_config;

		std::shared_ptr<spdlog::sinks::stdout_color_sink_mt>             m_consoleSink;
		std::shared_ptr<spdlog::sinks::basic_file_sink_mt>               m_fileSink;
		std::shared_ptr<spdlog::sinks::msvc_sink_mt>                     m_debugSink;

		std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> m_categoryLoggers;

		OnFatalCallback                                                  m_onFatalCallback;
		std::vector<spdlog::sink_ptr>                                    m_activeSinks;
	};

	namespace Internal
	{
		Logger* GetLoggerInstance();
	}
}

#include "Logging/Logger.inl"


#define RYU_LOG_TRACE(category, ...)                                                                                                   \
    do                                                                                                                                 \
	{                                                                                                                                  \
        if constexpr (static_cast<i32>(::Ryu::Logging::LogLevel::Trace) >= static_cast<i32>(::Ryu::Logging::COMPILE_TIME_LOG_LEVEL))   \
		{                                                                                                                              \
            if (auto* logger = ::Ryu::Logging::Internal::GetLoggerInstance())                                                          \
			{                                                                                                                          \
                logger->Trace(category, __VA_ARGS__);                                                                                  \
            }                                                                                                                          \
        }                                                                                                                              \
    } while(0)

#define RYU_LOG_DEBUG(category, ...)                                                                                                 \
    do                                                                                                                               \
	{                                                                                                                                \
        if constexpr (static_cast<i32>(::Ryu::Logging::LogLevel::Debug) >= static_cast<i32>(::Ryu::Logging::COMPILE_TIME_LOG_LEVEL)) \
		{                                                                                                                            \
            if (auto* logger = ::Ryu::Logging::Internal::GetLoggerInstance())                                                        \
			{                                                                                                                        \
                logger->Debug(category, __VA_ARGS__);                                                                                \
            }                                                                                                                        \
        }                                                                                                                            \
    } while(0)

#define RYU_LOG_INFO(category, ...)                                                                                                 \
    do                                                                                                                              \
	{                                                                                                                               \
        if constexpr (static_cast<i32>(::Ryu::Logging::LogLevel::Info) >= static_cast<i32>(::Ryu::Logging::COMPILE_TIME_LOG_LEVEL)) \
		{                                                                                                                           \
            if (auto* logger = ::Ryu::Logging::Internal::GetLoggerInstance())                                                       \
			{                                                                                                                       \
                logger->Info(category, __VA_ARGS__);                                                                                \
            }                                                                                                                       \
        }                                                                                                                           \
    } while(0)

#define RYU_LOG_WARN(category, ...)                                                                                                 \
    do                                                                                                                              \
	{                                                                                                                               \
        if constexpr (static_cast<i32>(::Ryu::Logging::LogLevel::Warn) >= static_cast<i32>(::Ryu::Logging::COMPILE_TIME_LOG_LEVEL)) \
		{                                                                                                                           \
            if (auto* logger = ::Ryu::Logging::Internal::GetLoggerInstance())                                                       \
			{                                                                                                                       \
                logger->Warn(category, __VA_ARGS__);                                                                                \
            }                                                                                                                       \
        }                                                                                                                           \
    } while(0)

#define RYU_LOG_ERROR(category, ...)                                      \
    do                                                                    \
	{                                                                     \
        if (auto* logger = ::Ryu::Logging::Internal::GetLoggerInstance()) \
		{                                                                 \
            logger->Error(category, __VA_ARGS__);                         \
        }                                                                 \
    } while(0)

#define RYU_LOG_FATAL(category, ...)                                      \
    do                                                                    \
	{                                                                     \
        if (auto* logger = ::Ryu::Logging::Internal::GetLoggerInstance()) \
		{                                                                 \
            logger->Fatal(category, __VA_ARGS__);                         \
        }                                                                 \
    } while(0)

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

namespace Ryu::Engine { class Engine; }

namespace Ryu::Logging
{
	class Logger
	{
		friend class Engine::Engine;
	public:
		using OnFatalCallback = std::function<void(LogLevel, const std::string&)>;

		RYU_API Logger();
		~Logger() = default;

		RYU_API void Configure(const LoggingConfig& config);
		RYU_API void SetRuntimeLogLevel(LogLevel level);
		inline LogLevel GetRuntimeLogLevel() const { return m_config.RuntimeLogLevel; }

		RYU_API void EnableSink(const std::string& sinkName, bool enable = true);
		RYU_API void DisableSink(const std::string& sinkName);
		RYU_API bool IsSinkEnabled(const std::string& sinkName) const;
		inline void SetOnFatalCallback(OnFatalCallback callback) { m_onFatalCallback = std::move(callback); }

		RYU_API std::shared_ptr<spdlog::logger> GetCategoryLogger(std::string_view category);

		RYU_API void Flush();

		template<typename... Args>
		void Log(std::string_view category, LogLevel level, fmt::format_string<Args...> format, Args&&... args);

		template<typename... Args>
		void Trace(std::string_view category, fmt::format_string<Args...> format, Args&&... args);

		template<typename... Args>
		void Debug(std::string_view category, fmt::format_string<Args...> format, Args&&... args);

		template<typename... Args>
		void Info(std::string_view category, fmt::format_string<Args...> format, Args&&... args);

		template<typename... Args>
		void Warn(std::string_view category, fmt::format_string<Args...> format, Args&&... args);

		template<typename... Args>
		void Error(std::string_view category, fmt::format_string<Args...> format, Args&&... args);

		template<typename... Args>
		void Fatal(std::string_view category, fmt::format_string<Args...> format, Args&&... args);

	private:
		void Shutdown();
		void CreateSinks(const LoggingConfig& config);
		void UpdateLoggers();
		std::string FormatCategoryName(std::string_view category) const;

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


#define RYU_LOG_TRACE(...)                                                                                                           \
    do                                                                                                                               \
	{                                                                                                                                \
        if constexpr (static_cast<i32>(::Ryu::Logging::LogLevel::Trace) >= static_cast<i32>(::Ryu::Logging::COMPILE_TIME_LOG_LEVEL)) \
		{                                                                                                                            \
            if (auto* logger = ::Ryu::Logging::Internal::GetLoggerInstance())                                                        \
			{                                                                                                                        \
                logger->Trace(RYU_FILE, __VA_ARGS__);                                                                            \
            }                                                                                                                        \
        }                                                                                                                            \
    } while(0)

#define RYU_LOG_DEBUG(...)                                                                                                           \
    do                                                                                                                               \
	{                                                                                                                                \
        if constexpr (static_cast<i32>(::Ryu::Logging::LogLevel::Debug) >= static_cast<i32>(::Ryu::Logging::COMPILE_TIME_LOG_LEVEL)) \
		{                                                                                                                            \
            if (auto* logger = ::Ryu::Logging::Internal::GetLoggerInstance())                                                        \
			{                                                                                                                        \
                logger->Debug(RYU_FILE, __VA_ARGS__);                                                                            \
            }                                                                                                                        \
        }                                                                                                                            \
    } while(0)

#define RYU_LOG_INFO(...)                                                                                                           \
    do                                                                                                                              \
	{                                                                                                                               \
        if constexpr (static_cast<i32>(::Ryu::Logging::LogLevel::Info) >= static_cast<i32>(::Ryu::Logging::COMPILE_TIME_LOG_LEVEL)) \
		{                                                                                                                           \
            if (auto* logger = ::Ryu::Logging::Internal::GetLoggerInstance())                                                       \
			{                                                                                                                       \
                logger->Info(RYU_FILE, __VA_ARGS__);                                                                            \
            }                                                                                                                       \
        }                                                                                                                           \
    } while(0)

#define RYU_LOG_WARN(...)                                                                                                           \
    do                                                                                                                              \
	{                                                                                                                               \
        if constexpr (static_cast<i32>(::Ryu::Logging::LogLevel::Warn) >= static_cast<i32>(::Ryu::Logging::COMPILE_TIME_LOG_LEVEL)) \
		{                                                                                                                           \
            if (auto* logger = ::Ryu::Logging::Internal::GetLoggerInstance())                                                       \
			{                                                                                                                       \
                logger->Warn(RYU_FILE, __VA_ARGS__);                                                                            \
            }                                                                                                                       \
        }                                                                                                                           \
    } while(0)

#define RYU_LOG_ERROR(...)                                                    \
    do                                                                        \
	{                                                                         \
        if (auto* logger = ::Ryu::Logging::Internal::GetLoggerInstance())     \
		{                                                                     \
            logger->Error(RYU_FILE, __VA_ARGS__);                         \
        }                                                                     \
    } while(0)

#define RYU_LOG_FATAL(...)                                                    \
    do                                                                        \
	{                                                                         \
        if (auto* logger = ::Ryu::Logging::Internal::GetLoggerInstance())     \
		{                                                                     \
            logger->Fatal(RYU_FILE, __VA_ARGS__);                         \
        }                                                                     \
    } while(0)

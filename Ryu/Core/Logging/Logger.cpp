#include "Core/Logging/Logger.h"
#include <spdlog/async.h>
#include <filesystem>
#include <chrono>

namespace Ryu::Logging
{
    namespace fs = std::filesystem;

    Logger s_logger;

    std::string LoggingConfig::GetSpdlogPattern() const
    {
        if (!Pattern.CustomPattern.empty())
        {
            return Pattern.CustomPattern;
        }

        // Formatting help: https://github.com/gabime/spdlog/wiki/Custom-formatting
        // TIMESTAMP | LEVEL | THREAD | CATEGORY: MESSAGE

        std::string pattern = "%^";  // Color output

        if (Pattern.IncludeTimestamp)
        {
            pattern += "%H:%M:%S.%e | ";
        }

        pattern += "%-7l";  // Level

        if (Pattern.IncludeThreadId)
        {
            pattern += " | [%t]";
        }

        pattern += " | %n%$: %v";

        return pattern;
    }

    Logger::Logger()
        : m_onFatalCallback([](LogLevel, const std::string&) { std::abort(); })
    {
    }

    void Logger::Configure(const LoggingConfig& config)
    {
        m_config = config;

        if (config.AsyncLogging)
        {
            spdlog::init_thread_pool(config.AsyncQueueSize, 1);
        }

        CreateSinks(config);
        UpdateLoggers();

        // Set global level
        spdlog::set_level(Internal::ToSpdlogLevel(config.RuntimeLogLevel));

        if (config.FlushOnError)
        {
            spdlog::flush_on(spdlog::level::err);
        }
    }

    void Logger::SetRuntimeLogLevel(LogLevel level)
    {
        m_config.RuntimeLogLevel = level;
        spdlog::set_level(Internal::ToSpdlogLevel(level));

        // Update all category loggers
        for (auto& [name, logger] : m_categoryLoggers)
        {
            logger->set_level(Internal::ToSpdlogLevel(level));
        }
    }

    void Logger::EnableSink(const std::string& sinkName, bool enable)
    {
        if (sinkName == "Console")
        {
            m_config.Sinks.Console = enable;
        }
        else if (sinkName == "File")
        {
            m_config.Sinks.File = enable;
        }
        else if (sinkName == "Debug")
        {
            m_config.Sinks.Debug = enable;
        }

        UpdateLoggers();
    }

    void Logger::DisableSink(const std::string& sinkName)
    {
        EnableSink(sinkName, false);
    }

    bool Logger::IsSinkEnabled(const std::string& sinkName) const
    {
        if (sinkName == "Console")
        {
            return m_config.Sinks.Console;
        }
        
        if (sinkName == "File")
        {
            return m_config.Sinks.File;
        }
        
        if (sinkName == "Debug")
        {
            return m_config.Sinks.Debug;
        }
        
        return false;
    }

    std::shared_ptr<spdlog::logger> Logger::GetCategoryLogger(std::string_view category)
    {
        std::string categoryName = FormatCategoryName(category);

        auto it = m_categoryLoggers.find(categoryName);
        if (it != m_categoryLoggers.end())
        {
            return it->second;
        }

        // Create new logger for this category
        std::shared_ptr<spdlog::logger> logger;

        if (m_config.AsyncLogging)
        {
            logger = std::make_shared<spdlog::async_logger>(
                categoryName,
                m_activeSinks.begin(),
                m_activeSinks.end(),
                spdlog::thread_pool(),
                spdlog::async_overflow_policy::block);
        }
        else
        {
            logger = std::make_shared<spdlog::logger>(
                categoryName,
                m_activeSinks.begin(),
                m_activeSinks.end());
        }

        logger->set_level(Internal::ToSpdlogLevel(m_config.RuntimeLogLevel));
        logger->set_pattern(m_config.GetSpdlogPattern());

        m_categoryLoggers[categoryName] = logger;
        spdlog::register_logger(logger);

        return logger;
    }

    void Logger::Flush()
    {
        for (auto& [name, logger] : m_categoryLoggers)
        {
            logger->flush();
        }
    }

    void Logger::CreateSinks(const LoggingConfig& config)
    {
        m_activeSinks.clear();

        // Console sink
        if (config.Sinks.Console)
        {
            if (!m_consoleSink)
            {
                m_consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                m_consoleSink->set_color_mode(spdlog::color_mode::always);
            }
            m_activeSinks.push_back(m_consoleSink);
        }

        // File sink
        if (config.Sinks.File)
        {
            // Ensure directory exists
            fs::path logPath(config.Sinks.LogFilePath);
            if (logPath.has_parent_path())
            {
                fs::create_directories(logPath.parent_path());
            }

            if (!m_fileSink)
            {
                m_fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                    config.Sinks.LogFilePath, true);
            }
            m_activeSinks.push_back(m_fileSink);
        }

        // Debug sink
        if (config.Sinks.Debug)
        {
            if (!m_debugSink)
            {
                m_debugSink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
            }
            m_activeSinks.push_back(m_debugSink);
        }
    }

    void Logger::UpdateLoggers()
    {
        // Update existing loggers with new sink configuration
        for (auto& [name, logger] : m_categoryLoggers)
        {
            logger->sinks().clear();

            for (auto& sink : m_activeSinks)
            {
                logger->sinks().push_back(sink);
            }
        }
    }

    void Logger::Shutdown()
    {
        spdlog::shutdown();
    }

    std::string Logger::FormatCategoryName(std::string_view category) const
    {
        return std::string(category);
    }

    Logger* Internal::GetLoggerInstance()
    {
        return &s_logger;
    }
}


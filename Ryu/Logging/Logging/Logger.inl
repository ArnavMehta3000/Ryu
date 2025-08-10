#include "Logger.h"
namespace Ryu::Logging
{
	template<typename ...Args>
	inline void Logger::Log(const LogCategory& category, LogLevel level, fmt::format_string<Args...> format, Args&&... args)
	{
		if (static_cast<i32>(level) < static_cast<i32>(m_config.RuntimeLogLevel)
			|| static_cast<i32>(level) < static_cast<i32>(COMPILE_TIME_LOG_LEVEL))
		{
			return;
		}

		auto logger = GetCategoryLogger(category);
		logger->log(Internal::ToSpdlogLevel(level), format, std::forward<Args>(args)...);

		if (level == LogLevel::Fatal && m_onFatalCallback)
		{
			std::string message = fmt::format(format, std::forward<Args>(args)...);
			m_onFatalCallback(level, message);
		}
	}
	
	template<typename ...Args>
	inline void Ryu::Logging::Logger::Trace(const LogCategory& category, fmt::format_string<Args...> format, Args&&... args)
	{
		Log(category, LogLevel::Trace, format, std::forward<Args>(args)...);
	}
	
	template<typename ...Args>
	inline void Logger::Debug(const LogCategory& category, fmt::format_string<Args...> format, Args&&... args)
	{
		Log(category, LogLevel::Debug, format, std::forward<Args>(args)...);
	}
	
	template<typename ...Args>
	inline void Logger::Info(const LogCategory& category, fmt::format_string<Args...> format, Args&&... args)
	{
		Log(category, LogLevel::Info, format, std::forward<Args>(args)...);
	}
	
	template<typename ...Args>
	inline void Logger::Warn(const LogCategory& category, fmt::format_string<Args...> format, Args&&... args)
	{
		Log(category, LogLevel::Warn, format, std::forward<Args>(args)...);
	}
	
	template<typename ...Args>
	inline void Logger::Error(const LogCategory& category, fmt::format_string<Args...> format, Args&&... args)
	{
		Log(category, LogLevel::Error, format, std::forward<Args>(args)...);
	}
	
	template<typename ...Args>
	inline void Logger::Fatal(const LogCategory& category, fmt::format_string<Args...> format, Args&&... args)
	{
		Log(category, LogLevel::Fatal, format, std::forward<Args>(args)...);
	}
}

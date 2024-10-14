#include "Logger.h"
#include <fmt/core.h>
#include <fmt/chrono.h>

namespace Ryu::Logging
{
	Logger& Logger::Get()
	{
		static Logger instance;
		return instance;
	}

	void Logger::AddSink(std::unique_ptr<ILogSink> sink)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_sinks.push_back(std::move(sink));
	}

	void Logger::Log(const LogCategory& category, LogLevel level, const std::string& message) const
	{
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);

		std::tm timeInfo;
		localtime_s(&timeInfo, &time);

		std::string timeStr = fmt::format("{:%Y-%m-%d %H:%M:%S}", timeInfo);

		std::string formattedMessage = fmt::format("[{}] [{}] [{}]: {}\n", 
			timeStr,
			category.Name,
			EnumToString(level),
			message);

		std::lock_guard<std::mutex> lock(m_mutex);
		for (const auto& sink : m_sinks)
		{
			sink->Log(level, formattedMessage);
		}

		// Check if fatal error
		if (level == LogLevel::Fatal)
		{
			if (m_onFatalCallback)
			{
				// Just pass the raw-unformatted message
				m_onFatalCallback(level, message);
			}
		}
	}
}

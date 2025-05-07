#include "Logger.h"
#include "Common/Enum.h"
#include <fmt/core.h>
#include <fmt/chrono.h>

namespace Ryu::Logging
{
	void Logger::AddSink(std::unique_ptr<ILogSink> sink)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_sinks.push_back(std::move(sink));
	}

	void Logger::Log(const LogCategory& category, LogLevel level, const LogMessage& message) const
	{
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);

		std::tm timeInfo;
		localtime_s(&timeInfo, &time);

		std::string timeStr = fmt::format("{:%Y-%m-%d %H:%M:%S}", timeInfo);

		std::string formattedMessage;

		// Add stacktrace if needed
		if (level == LogLevel::Fatal)
		{
			auto entry = *message.Stacktrace.begin();
			formattedMessage = std::format("[{}] [{}] [{}]: {}\n{}({}):{}", timeStr,
				category.Name,
				EnumToString(level),
				message.Message,
				entry.source_file(),
				entry.source_line(),
				entry.description());
		}
		else
		{
			formattedMessage = fmt::format("[{}] [{}] [{}]: {}",
				timeStr,
				category.Name,
				EnumToString(level),
				message.Message);
		}

		std::lock_guard<std::mutex> lock(m_mutex);
		for (const auto& sink : m_sinks)
		{
			sink->Log(level, LogMessage{ formattedMessage, message.Stacktrace });
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

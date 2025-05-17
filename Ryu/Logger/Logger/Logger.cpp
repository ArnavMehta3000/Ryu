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
		std::lock_guard<std::mutex> lock(m_mutex);
		for (const auto& sink : m_sinks)
		{
			sink->Log(category, level, message);
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
	
	FormattedLogMessage DefaultFormatter(const LogCategory& category, LogLevel level, const LogMessage& message, bool captureStackTrace)
	{
		FormattedLogMessage output{};

		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);

		std::tm timeInfo;
		localtime_s(&timeInfo, &time);

		output.Timestamp = fmt::format("{:%Y-%m-%d %H:%M:%S}", timeInfo);
		output.Category  = category.Name;
		output.Message   = message.Message;

		std::string logLevelStr(EnumToString(level));
		std::transform(logLevelStr.begin(), logLevelStr.end(), logLevelStr.begin(), [](unsigned char c) { return std::toupper(c); });
		output.Level = logLevelStr;

		if (captureStackTrace)
		{
			const auto entry = *message.Stacktrace.begin();
			output.Stacktrace = std::format("{}({}) : {}",
				entry.source_file(),
				entry.source_line(),
				entry.description());
		}

		return output;
	}
	
	std::string DefaultFormatter(LogLevel level, const FormattedLogMessage& message)
	{
		std::string formattedMessage;

		// Add stacktrace if needed
		if (level == LogLevel::Fatal)
		{
			formattedMessage = std::format("[{}] [{}] [{}]: {}\n{}",
				message.Timestamp,
				message.Level,
				message.Category,
				message.Message,
				message.Stacktrace);
		}
		else
		{
			formattedMessage = std::format("[{}] [{}] [{}]: {}",
				message.Timestamp,
				message.Level,
				message.Category,
				message.Message);
		}

		return formattedMessage;
	}
}

#include "ConsoleSink.h"
#include "Logger/Logger.h"
#include <print>

namespace Ryu::Logging
{
	ConsoleSink::ConsoleSink()
		: m_file(nullptr)
	{
		// Attach console if not attached
		if (!AttachConsole(GetCurrentProcessId()))
		{
			AllocConsole();
		}

		// Redirect stdout to the new console
		freopen_s(&m_file, "CONOUT$", "w", stdout);

		m_defaultConsoleAttributes = GetDefaultConsoleAttributes();
	}

	ConsoleSink::~ConsoleSink()
	{
		if (m_file)
		{
			fclose(m_file);
			FreeConsole();
		}
	}

	void ConsoleSink::Log(const LogCategory&, LogLevel level, const LogMessage&, const FormattedLogMessage& formattedMsg) const
	{
		HANDLE hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);

		// For Error and Fatal levels, color the entire line
		if (level == LogLevel::Error || level == LogLevel::Fatal)
		{
			SetConsoleColor(level);
			std::println(stdout, "[{}] [{}] [{}]: {}{}",
				formattedMsg.Timestamp,
				formattedMsg.Level,
				formattedMsg.Category,
				formattedMsg.Message,
				formattedMsg.Stacktrace.empty() ? "" : ("\n" + formattedMsg.Stacktrace));

			SetConsoleTextAttribute(hConsole, m_defaultConsoleAttributes);
			return;
		}

		// For other levels, color only the level  and categrory part
		std::print(stdout, "[{}] ", formattedMsg.Timestamp);

		SetConsoleColor(level);
		std::print(stdout, "[{}] [{}]: ", formattedMsg.Level, formattedMsg.Category);
		SetConsoleTextAttribute(hConsole, m_defaultConsoleAttributes);

		std::println(stdout, "{}",formattedMsg.Message);
	}

	std::string_view ConsoleSink::GetName() const
	{
		return "Console";
	}

	WORD ConsoleSink::GetDefaultConsoleAttributes() const
	{
		constexpr WORD DEFALT_CONSOLE_ATTRIBUTES = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;

		CONSOLE_SCREEN_BUFFER_INFO info{};
		if (::GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
		{
			return info.wAttributes == 0 ? DEFALT_CONSOLE_ATTRIBUTES : info.wAttributes;
		}

		return DEFALT_CONSOLE_ATTRIBUTES;
	}

	void ConsoleSink::SetConsoleColor(LogLevel level) const
	{
		HANDLE hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
		WORD color = m_defaultConsoleAttributes;  // Default color

		switch (level)
		{
			using enum LogLevel;
		case Trace: color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;                                         break;
		case Debug: color = FOREGROUND_GREEN | FOREGROUND_BLUE;                                                          break;
		case Info:  color = FOREGROUND_GREEN;                                                                            break;
		case Warn:  color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;                                    break;
		case Error: color = FOREGROUND_INTENSITY | FOREGROUND_RED;                                                       break;
		case Fatal: color = BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
		}

		::SetConsoleTextAttribute(hConsole, color);
	}

}

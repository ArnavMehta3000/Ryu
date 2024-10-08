#include "ConsoleSink.h"
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
		}
	}

	void ConsoleSink::Log(LogLevel level, const std::string& message) const
	{
		SetConsoleColor(level);
		std::print(stdout, "{}", message);

		// Reset to default color
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), m_defaultConsoleAttributes);
	}

	WORD ConsoleSink::GetDefaultConsoleAttributes() const
	{
		CONSOLE_SCREEN_BUFFER_INFO info;
		::GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
		return info.wAttributes;
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

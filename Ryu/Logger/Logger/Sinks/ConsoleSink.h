#pragma once
#include "Logger/Sinks/ILogSink.h"
#include <Windows.h>

namespace Ryu::Logging
{
	class ConsoleSink : public ILogSink
	{
	public:
		ConsoleSink();
		~ConsoleSink();

		void Log(LogLevel level, const std::string& message) const override;

	private:
		WORD GetDefaultConsoleAttributes() const;
		void SetConsoleColor(LogLevel level) const;

	private:
		FILE* m_file;
		WORD m_defaultConsoleAttributes;
	};
}

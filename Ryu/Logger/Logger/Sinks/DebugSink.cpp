#include "DebugSink.h"
#include "Logger/Logger.h"
#include <Windows.h>

namespace Ryu::Logging
{
	void DebugSink::Log(const LogCategory&, LogLevel level, const LogMessage&, const FormattedLogMessage& formattedMsg) const
	{
		::OutputDebugStringA((DefaultFormatter(level, formattedMsg) + "\n").c_str());
	}

	std::string_view DebugSink::GetName() const
	{
		return "Debug";
	}
}

#include "DebugSink.h"
#include "Common/ObjectMacros.h"
#include "Logger/Logger.h"
#include <Windows.h>

namespace Ryu::Logging
{
	void DebugSink::Log(const LogCategory& category, LogLevel level, const LogMessage& message) const
	{
		::OutputDebugStringA((DefaultFormatter(level, DefaultFormatter(category, level, message, level == LogLevel::Fatal)) + "\n").c_str());
	}

	std::string_view DebugSink::GetName() const
	{
		return "Debug";
	}
}

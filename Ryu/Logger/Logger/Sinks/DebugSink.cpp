#include "DebugSink.h"
#include "Common/ObjectMacros.h"
#include <Windows.h>

namespace Ryu::Logging
{
	void DebugSink::Log(MAYBE_UNUSED LogLevel level, const LogMessage& message) const
	{
		::OutputDebugStringA((message.Message + "\n").c_str());
	}

	std::string_view DebugSink::GetName() const
	{
		return "Debug";
	}
}

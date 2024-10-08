#include "DebugSink.h"
#include "Common/ObjectMacros.h"
#include <Windows.h>

namespace Ryu::Logging
{
	void DebugSink::Log(MAYBE_UNUSED LogLevel level, const std::string& message) const
	{
		::OutputDebugStringA(message.c_str());
	}
}

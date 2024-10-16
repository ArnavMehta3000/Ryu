#pragma once
#include <string>
#include <stacktrace>

namespace Ryu::Logging
{
	struct LogMessage
	{
		std::string Message;
		std::stacktrace Stacktrace;
	};
}

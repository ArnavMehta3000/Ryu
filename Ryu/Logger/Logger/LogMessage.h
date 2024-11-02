#pragma once
#include <string>
#include <stacktrace>

namespace Ryu::Logging
{
	/**
	 * @brief Contains a message and a stacktrace
	 */
	struct LogMessage
	{
		std::string Message;
		std::stacktrace Stacktrace;
	};
}

#pragma once
#include "Common/Enum.h"

namespace Ryu::Logging
{
	enum class LogLevel
	{
		Trace = 0,
		Debug = 1,
		Info = 2,
		Warn = 3,
		Error = 4,
		Fatal = 5
	};
}

RYU_BEGIN_ENUM_TO_STRING(Ryu::Logging::LogLevel)
	RYU_ENUM_TO_STRING(Trace)
	RYU_ENUM_TO_STRING(Debug)
	RYU_ENUM_TO_STRING(Info)
	RYU_ENUM_TO_STRING(Warn)
	RYU_ENUM_TO_STRING(Error)
	RYU_ENUM_TO_STRING(Fatal)
RYU_END_ENUM_TO_STRING()

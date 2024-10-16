#pragma once
#include "Logger/LogLevel.h"
#include "Logger/LogMessage.h"
#include <string>

namespace Ryu::Logging
{
	class ILogSink
	{
	public:
		virtual ~ILogSink() = default;
		virtual void Log(LogLevel level, const LogMessage& message) const = 0;
		virtual std::string_view GetName() const = 0;
	};
}

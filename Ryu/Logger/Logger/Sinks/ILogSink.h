#pragma once
#include "Logger/LogLevel.h"
#include <string>

namespace Ryu::Logging
{
	class ILogSink
	{
	public:
		virtual ~ILogSink() = default;
		virtual void Log(LogLevel level, const std::string& message) const = 0;
		virtual std::string_view GetName() const = 0;
	};
}

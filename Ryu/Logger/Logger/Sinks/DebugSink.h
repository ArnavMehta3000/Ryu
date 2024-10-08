#pragma once
#include "Logger/Sinks/ILogSink.h"


namespace Ryu::Logging
{
	class DebugSink : public ILogSink
	{
	public:
		void Log(LogLevel level, const std::string& message) const override;
	};
}

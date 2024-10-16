#pragma once
#include "Logger/Sinks/ILogSink.h"


namespace Ryu::Logging
{
	class DebugSink : public ILogSink
	{
	public:
		void Log(LogLevel level, const LogMessage& message) const override;
		std::string_view GetName() const override;
	};
}

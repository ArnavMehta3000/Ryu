#pragma once
#include "Logger/Sinks/ILogSink.h"
#include <functional>

namespace Ryu::Logging
{
	class CallbackSink : public ILogSink
	{
	public:
		explicit CallbackSink(std::function<void(LogLevel, const LogMessage&)> callback);
		
		void Log(LogLevel level, const LogMessage& message) const override;
		std::string_view GetName() const override;

	private:
		std::function<void(LogLevel, const LogMessage&)> m_callback;
	};
}

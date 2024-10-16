#include "CallbackSink.h"

namespace Ryu::Logging
{
	CallbackSink::CallbackSink(std::function<void(LogLevel, const LogMessage&)> callback)
		: m_callback(std::move(callback))
	{
	}

	void CallbackSink::Log(LogLevel level, const LogMessage& message) const
	{
		if (m_callback)
		{
			m_callback(level, message);
		}
	}

	std::string_view CallbackSink::GetName() const
	{
		return "Callback";
	}
}

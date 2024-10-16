#include "CallbackSink.h"

namespace Ryu::Logging
{
	CallbackSink::CallbackSink(std::function<void(LogLevel, const std::string&)> callback)
		: m_callback(std::move(callback))
	{
	}

	void CallbackSink::Log(LogLevel level, const std::string& message) const
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

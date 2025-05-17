#include "CallbackSink.h"
#include "Logger/Logger.h"

namespace Ryu::Logging
{
	CallbackSink::CallbackSink(std::function<void(const LogCategory&, LogLevel, const LogMessage&, const FormattedLogMessage&)> callback)
		: m_callback(std::move(callback))
	{
	}

	void CallbackSink::Log(const LogCategory& category, LogLevel level, const LogMessage& message, const FormattedLogMessage& formattedMsg) const
	{
		if (m_callback)
		{
			m_callback(category, level, message, formattedMsg);
		}
	}

	std::string_view CallbackSink::GetName() const
	{
		return "Callback";
	}
}

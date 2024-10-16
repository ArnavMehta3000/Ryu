#pragma once
#include "Logger/Sinks/ILogSink.h"
#include <functional>

namespace Ryu::Logging
{
	class CallbackSink : public ILogSink
	{
	public:
		explicit CallbackSink(std::move_only_function<void(LogLevel, const std::string&)> callback);
		
		void Log(LogLevel level, const std::string& message) const override;
		std::string_view GetName() const override;

	private:
		std::function<void(LogLevel, const std::string&)> m_callback;
	};
}

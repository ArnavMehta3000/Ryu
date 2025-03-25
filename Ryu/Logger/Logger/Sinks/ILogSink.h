#pragma once
#include "Common/API.h"
#include "Logger/LogLevel.h"
#include "Logger/LogMessage.h"
#include <string>

namespace Ryu::Logging
{
	/**
	 * @brief Interface for logging sinks.
	 */
	class RYU_API ILogSink
	{
	public:
		/**
		 * @brief Virtual destructor.
		 */
		virtual ~ILogSink() = default;

		/**
		 * @brief Called when a log message is dispatched to the sink
		 * @param level The `LogLevel` of the message
		 * @param message The `LogMessage` of the message
		 */
		virtual void Log(LogLevel level, const LogMessage& message) const = 0;

		/**
		 * @brief Returns the name of the sink
		 * @return The name of the sink
		 */
		virtual std::string_view GetName() const = 0;
	};
}

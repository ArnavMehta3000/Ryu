#pragma once
#include "Common/API.h"
#include "Logger/LogLevel.h"
#include "Logger/LogMessage.h"
#include "Logger/LogCategory.h"
#include <string>

namespace Ryu::Logging
{
	struct FormattedLogMessage;

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
		 * @param formattedMsg The `FormattedLogMessage` of the message
		 */
		virtual void Log(const LogCategory& category, LogLevel level, const LogMessage& message, const FormattedLogMessage& formattedMsg) const = 0;

		/**
		 * @brief Returns the name of the sink
		 * @return The name of the sink
		 */
		virtual std::string_view GetName() const = 0;
	};
}

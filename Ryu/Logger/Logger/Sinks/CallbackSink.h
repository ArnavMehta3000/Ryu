#pragma once
#include "Logger/Sinks/ILogSink.h"
#include <functional>

namespace Ryu::Logging
{
	/**
	 * @brief An sink that writes log messages to a callback function
	 */
	class CallbackSink : public ILogSink
	{
	public:
		/**
		 * @brief Constructs a new CallbackSink
		 * @param callback The callback function to write log messages to
		 */
		RYU_API explicit CallbackSink(std::function<void(const LogCategory&, LogLevel, const LogMessage&, const FormattedLogMessage&)> callback);

		/**
		 * @brief Writes a log message to the file
		 * @details Overrides the `Log` method of the `ILogSink` interface
		 * @param level The `LogLevel` of the message
		 * @param message The `LogMessage` to write
		 */
		RYU_API void Log(const LogCategory& category, LogLevel level, const LogMessage& message, const FormattedLogMessage& formattedMsg) const override;

		/**
		 * @brief Returns the name of the sink
		 * @details Overrides the `GetName` method of the `ILogSink` interface
		 * @return The name of the sink
		 */
		RYU_API std::string_view GetName() const override;

	private:
		std::function<void(const LogCategory&, LogLevel, const LogMessage&, const FormattedLogMessage&)> m_callback;
	};
}

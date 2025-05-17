#pragma once
#include "Logger/Sinks/ILogSink.h"


namespace Ryu::Logging
{
	/**
	 * @brief Writes a message to the debug output stream.
	 * @details Log message will be visible in the debugger's output window.
	 */
	class DebugSink : public ILogSink
	{
	public:
		/**
		 * @brief Writes a log message to the file
		 * @details Overrides the `Log` method of the `ILogSink` interface
		 * @param level The `LogLevel` of the message
		 * @param message The `LogMessage` to write
		 */
		RYU_API void Log(const LogCategory& category, LogLevel level, const LogMessage& message) const override;

		/**
		 * @brief Returns the name of the sink
		 * @details Overrides the `GetName` method of the `ILogSink` interface
		 * @return The name of the sink
		 */
		RYU_API std::string_view GetName() const override;
	};
}

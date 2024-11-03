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
		RYU_API explicit CallbackSink(std::function<void(LogLevel, const LogMessage&)> callback);
		
		/**
		 * @brief Writes a log message to the file
		 * @details Overrides the `Log` method of the `ILogSink` interface
		 * @param level The `LogLevel` of the message
		 * @param message The `LogMessage` to write
		 */
		RYU_API void Log(LogLevel level, const LogMessage& message) const override;

		/**
		 * @brief Returns the name of the sink
		 * @details Overrides the `GetName` method of the `ILogSink` interface
		 * @return The name of the sink
		 */
		RYU_API std::string_view GetName() const override;

	private:
		std::function<void(LogLevel, const LogMessage&)> m_callback;
	};
}

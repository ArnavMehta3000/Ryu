#pragma once
#include "Logger/Sinks/ILogSink.h"
#include <Windows.h>

namespace Ryu::Logging
{
	/**
	 * @brief A sink that outputs log messages to the console
	 */
	class RYU_API ConsoleSink : public ILogSink
	{
	public:
		/**
		 * @brief Creates a new console sink
		 * @details This will also create a new console and redirect stdout to it
		 */
		ConsoleSink();

		/**
		 * @brief Destroys the console
		 */
		~ConsoleSink();

		/**
		 * @brief Writes a log message to the file
		 * @details Overrides the `Log` method of the `ILogSink` interface
		 * @param level The `LogLevel` of the message
		 * @param message The `LogMessage` to write
		 */
		void Log(const LogCategory& category, LogLevel level, const LogMessage& message, const FormattedLogMessage& formattedMsg) const override;

		/**
		 * @brief Returns the name of the sink
		 * @details Overrides the `GetName` method of the `ILogSink` interface
		 * @return The name of the sink
		 */
		std::string_view GetName() const override;

	private:
		WORD GetDefaultConsoleAttributes() const;
		void SetConsoleColor(LogLevel level) const;

	private:
		FILE* m_file;
		WORD m_defaultConsoleAttributes;
	};
}

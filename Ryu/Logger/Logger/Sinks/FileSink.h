#pragma once
#include "Logger/Sinks/ILogSink.h"
#include <fstream>

namespace Ryu::Logging
{
	/**
	 * @brief Writes log messages to a file
	 */
	class FileSink : public ILogSink
	{
	public:
		/**
		 * @brief Constructs a new FileSink object
		 * @param filename The name of the file to write to
		 */
		explicit FileSink(const std::string_view& filename);

		/**
		 * @brief Writes a log message to the file
		 * @details Overrides the `Log` method of the `ILogSink` interface
		 * @param level The `LogLevel` of the message
		 * @param message The `LogMessage` to write
		 */
		void Log(LogLevel level, const LogMessage& message) const override;

		/**
		 * @brief Returns the name of the sink
		 * @details Overrides the `GetName` method of the `ILogSink` interface
		 * @return The name of the sink
		 */
		std::string_view GetName() const override;

	private:
		mutable std::ofstream m_file;
	};
}

#pragma once
#include "Common/Enum.h"

namespace Ryu::Logging
{
	/**
	 * @brief The log level
	 * @details Dictates the color of the log message
	 */
	enum class LogLevel
	{
		/**
		 * @brief The trace log level
		 * @details The most verbose log level
		 */
		Trace = 0,

		/**
		 * @brief The debug log level
		 * @details The second most verbose log level. Contains debug information
		 */
		Debug = 1,

		/**
		 * @brief The info log level
		 * @details The default log level
		 */
		Info = 2,

		/**
		 * @brief The warn log level
		 * @details Contains warnings
		 */
		Warn = 3,

		/**
		 * @brief The error log level
		 * @details Contains errors and gives a stacktrace
		 */
		Error = 4,

		/**
		 * @brief The fatal log level
		 * @details Contains errors, gives a stacktrace and breaks the program
		 */
		Fatal = 5
	};
}

RYU_BEGIN_ENUM_TO_STRING(Ryu::Logging::LogLevel)
	RYU_ENUM_TO_STRING(Trace)
	RYU_ENUM_TO_STRING(Debug)
	RYU_ENUM_TO_STRING(Info)
	RYU_ENUM_TO_STRING(Warn)
	RYU_ENUM_TO_STRING(Error)
	RYU_ENUM_TO_STRING(Fatal)
RYU_END_ENUM_TO_STRING()

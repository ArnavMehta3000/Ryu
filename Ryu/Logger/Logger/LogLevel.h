#pragma once
#include <string_view>

namespace Ryu::Logging
{
	enum class LogLevel
	{
		Trace = 0,
		Debug = 1,
		Info = 2,
		Warn = 3,
		Error = 4,
		Fatal = 5
	};
}

inline constexpr std::string_view EnumToString(Ryu::Logging::LogLevel level)
{
	switch (level)
	{
		using enum Ryu::Logging::LogLevel;

	case Trace: return "Trace";
	case Debug: return "Debug";
	case Info:  return "Info";
	case Warn:  return "Warn";
	case Error: return "Error";
	case Fatal: return "Fatal";
	default:    return "Unknown";
	}
}

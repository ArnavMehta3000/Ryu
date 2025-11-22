#pragma once
#include "Globals/Globals.h"
#include "Config/Config.h"
#include <array>

namespace Ryu::Engine
{
	class Settings : Config::ConfigBase
	{
		RYU_CONFIG_DECLARE(Settings, "EngineSettings.toml");

	public:
		Config::ConfigValue<i32> LogLevel                { this, "Logging", "LogLevel", 1                      };
		Config::ConfigValue<bool> LogToConsole           { this, "Logging", "LogToConsole", Globals::g_isDebug };
		Config::ConfigValue<bool> LogToFile              { this, "Logging", "LogToFile", false                 };
		Config::ConfigValue<bool> LogThreadId            { this, "Logging", "LogThreadID", false               };
		Config::ConfigValue<bool> AsyncLogging           { this, "Logging", "EnableAsyncLogging", false        };
		Config::ConfigValue<std::string> LogCustomPatten { this, "Logging", "CustomLogPatttern", ""            };
	};
}

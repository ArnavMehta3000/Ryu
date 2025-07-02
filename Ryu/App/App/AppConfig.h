#pragma once
#include "Globals/Globals.h"
#include "Config/Config.h"

namespace Ryu::App
{
	class AppConfig : Config::ConfigBase
	{
		RYU_CONFIG_DECLARE(AppConfig, "AppConfig.toml");
		RYU_CONFIG_DECLARE_SECTION(Log);

	public:
		Config::ConfigValue<bool> EnableLogToConsole { this, LogSection, "EnableLogToConsole", Globals::g_isDebug };
		Config::ConfigValue<bool> EnableLogToFile    { this, LogSection, "EnableLogToFile", Globals::g_isDebug    };
		Config::ConfigValue<bool> ForceLogToOutput   { this, LogSection, "ForceLogToOutput", Globals::g_isDebug   };  // Force create debug sink even when debugger is not attached
		Config::ConfigValue<std::string> LogFilePath { this, LogSection, "LogFilePath", "Logs/Log.txt"            };
	};
}

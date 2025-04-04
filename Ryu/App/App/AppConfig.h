#pragma once
#include "Common/StandardTypes.h"
#include "Common/Globals.h"
#include "Config/Config.h"

namespace Ryu::App
{
	class AppConfig : Config::ConfigBase
	{
		RYU_DECLARE_CONFIG(AppConfig, "AppConfig.toml");
		RYU_DECLARE_CONFIG_SECTION(Window);
		RYU_DECLARE_CONFIG_SECTION(Debugging);
		RYU_DECLARE_CONFIG_SECTION(Log);

	public:
		Config::ConfigValue<std::string> WindowTitle         { this, WindowSection, "WindowTitle", "Ryu Window"                                                      };
		Config::ConfigValue<std::array<u32, 2>> WindowSize   { this, WindowSection, "WindowSize", { 1280, 720 }                                                      };												     
		Config::ConfigValue<std::array<u32, 2>> WindowMinSize{ this, WindowSection, "WindowMinimumSize", { 800, 600  }                                               };
		Config::ConfigValue<std::array<i32, 2>> WindowPos    { this, WindowSection, "WindowPosition", { static_cast<i32>(0x80000000), static_cast<i32>(0x80000000) } };  // Same as CW_USEDEFAULT;
		Config::ConfigValue<bool> EscToClose                 { this, DebuggingSection, "PressEscapeToClose", Common::Globals::g_isDebug                              };  // Only valid in debug builds (see Application::OnWindowKeyPressedEvent)
		Config::ConfigValue<bool> EnableLogToConsole         { this, LogSection, "EnableLogToConsole", Common::Globals::g_isDebug                                    };
		Config::ConfigValue<bool> EnableLogToFile            { this, LogSection, "EnableLogToFile", Common::Globals::g_isDebug                                       };
		Config::ConfigValue<bool> ForceLogToOutput           { this, LogSection, "ForceLogToOutput", Common::Globals::g_isDebug                                      };  // Force create debug sink even when debugger is not attached
		Config::ConfigValue<std::string> LogFilePath         { this, LogSection, "LogFilePath", "Logs/Log.txt"                                                       };
	};
}

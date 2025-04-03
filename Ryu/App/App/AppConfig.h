#pragma once
#include "Common/StandardTypes.h"
#include "Common/Globals.h"
#include "Config/Config.h"

namespace Ryu::App
{
	class AppConfig : Config::ConfigBase
	{
		RYU_DECLARE_CONFIG(AppConfig);
		RYU_DECLARE_CONFIG_SECTION(Window);
		RYU_DECLARE_CONFIG_SECTION(Debugging);
		RYU_DECLARE_CONFIG_SECTION(Log);

	public:
		AppConfig() 
			: Config::ConfigBase("AppConfig.toml")
			, WindowTitle(this, WindowSection, "WindowTitle", "Ryu Window")
			, WindowSize(this, WindowSection, "WindowSize", { 1280, 720 })
			, WindowMinSize(this, WindowSection, "WindowMinimumSize", { 800, 600 })
			, WindowPos(this, WindowSection, "WindowPosition", { static_cast<i32>(0x80000000), static_cast<i32>(0x80000000) })  // Same as CW_USEDEFAULT
			, EscToClose(this, DebuggingSection, "PressEscapeToClose", Common::Globals::g_isDebug)
			, EnableLogToConsole(this, LogSection, "EnableLogToConsole", Common::Globals::g_isDebug)
			, EnableLogToFile(this, LogSection, "EnableLogToFile", Common::Globals::g_isDebug)
			, ForceLogToOutput(this, LogSection, "ForceLogToOutput", Common::Globals::g_isDebug)
			, LogFilePath(this, LogSection, "LogFilePath", "Logs/Log.txt")
		{
		}
	
	public:
		Config::ConfigValue<std::string> WindowTitle;
		Config::ConfigValue<std::array<u32, 2>> WindowSize;
		Config::ConfigValue<std::array<u32, 2>> WindowMinSize;
		Config::ConfigValue<std::array<i32, 2>> WindowPos;
		Config::ConfigValue<bool> EscToClose;  // Only valid in debug builds (see Application::OnWindowKeyPressedEvent)
		Config::ConfigValue<bool> EnableLogToConsole;
		Config::ConfigValue<bool> EnableLogToFile;
		Config::ConfigValue<bool> ForceLogToOutput;  // Force create debug sink even when debugger is not attached
		Config::ConfigValue<std::string> LogFilePath;
	};
}

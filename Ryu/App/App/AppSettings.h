#pragma once
#include "Config/Config.h"
#include <array>

namespace Ryu::App
{
	class Settings : Config::ConfigBase
	{
		RYU_CONFIG_DECLARE(AppConfig, "AppConfig.toml");

	public:
		Config::ConfigValue<std::string> WindowTitle          { this, "Window", "WindowTitle", "Ryu Window"                           };
		Config::ConfigValue<std::array<i32, 2>> WindowSize    { this, "Window", "WindowSize", { -1, -1                              } };
		Config::ConfigValue<std::array<i32, 2>> WindowMinSize { this, "Window", "WindowMinSize", { 800, 600                         } };
		Config::ConfigValue<std::array<i32, 2>> WindowPos     { this, "Window", "WindowPos", { ((i32)0x80000000), ((i32)0x80000000) } };  // Default is CW_USEDEFAULT
		Config::ConfigValue<bool> CanBorderlessResize         { this, "Window Borderless", "EnableBorderlessResizing", true           };
		Config::ConfigValue<bool> CanBorderlessDrag           { this, "Window Borderless", "EnableBorderlessDrag", false              };
	};
}

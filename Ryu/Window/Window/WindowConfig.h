#pragma once
#include "Common/StandardTypes.h"
#include "Config/Config.h"
#include <string>

namespace Ryu::Window
{
	class WindowConfig : Config::ConfigBase
	{
		RYU_CONFIG_DECLARE(WindowConfig, "WindowConfig.toml");
		RYU_CONFIG_DECLARE_SECTION(Size);
		RYU_CONFIG_DECLARE_SECTION(Position);
		RYU_CONFIG_DECLARE_SECTION(Properties);
	public:

		Config::ConfigValue<i32> Width             { this, SizeSection, "Width", 1280                            };
		Config::ConfigValue<i32> Height            { this, SizeSection, "Height", 720                            };
		Config::ConfigValue<i32> X                 { this, PositionSection, "PosX", static_cast<i32>(0x80000000) };
		Config::ConfigValue<i32> Y                 { this, PositionSection, "PosY", static_cast<i32>(0x80000000) };
		Config::ConfigValue<std::string> Title     { this, PropertiesSection, "Title", "Ryu App Window"          };
		Config::ConfigValue<bool> IsResizable      { this, PropertiesSection, "IsResizable", true                };
		Config::ConfigValue<bool> HasMinimizeButton{ this, PropertiesSection, "HasMinimizeButton", true          };
		Config::ConfigValue<bool> HasMaximizeButton{ this, PropertiesSection, "HasMaximizeButton", true          };
		Config::ConfigValue<bool> HasCloseButton   { this, PropertiesSection, "HasCloseButton", true             };
		Config::ConfigValue<bool> IsVisible        { this, PropertiesSection, "IsVisible", true                  };
	};
}

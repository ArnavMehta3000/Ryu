#pragma once
#include "Globals/Globals.h"
#include "Config/Config.h"

namespace Ryu::Gfx
{
	class Settings : Config::ConfigBase
	{
		RYU_CONFIG_DECLARE(Settings, "GraphicsSettings.toml");

	public:
		Config::ConfigValue<bool> EnableDebugLayer{ this, "Debug", "EnableDebugLayer", Globals::g_isDebug };
		Config::ConfigValue<bool> EnableValidation{ this, "Debug", "EnableDebugValidation", true          };
		Config::ConfigValue<bool> AllowTearing    { this, "Rendering", "AllowTearing", true               };
		Config::ConfigValue<bool> EnableVSync     { this, "Rendering", "EnableVsync", true                };
	};
}

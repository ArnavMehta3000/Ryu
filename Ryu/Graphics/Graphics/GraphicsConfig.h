#pragma once
#include "Common/Globals.h"
#include "Common/StandardTypes.h"
#include "Config/Config.h"

namespace Ryu::Gfx
{
	class GraphicsConfig : Config::ConfigBase
	{
		RYU_CONFIG_DECLARE(GraphicsConfig, "GraphicsConfig.toml");
		RYU_CONFIG_DECLARE_SECTION(Rendering);
		RYU_CONFIG_DECLARE_SECTION(Debugging);

	public:
		Config::ConfigValue<bool> EnableDebugLayer{ this, DebuggingSection, "EnableDebugLayer", Common::Globals::g_isDebug };
		Config::ConfigValue<bool> EnableGPUBasedValidation{ this, DebuggingSection, "EnableGPUBasedValidation", false };
		Config::ConfigValue<bool> EnableVSync{ this, RenderingSection, "EnableVSync", true };
		Config::ConfigValue<bool> AllowTearing{ this, RenderingSection, "AllowTearing", false };
		Config::ConfigValue<bool> UseWARP{ this, RenderingSection, "UseWARP", false };

		static constexpr u32 FRAME_COUNT = 3;
	};
}

#pragma once
#include "Common/Globals.h"
#include "Config/Config.h"
#include "GraphicsRHI/GraphicsAPI.h"

namespace Ryu::Graphics
{
	class GraphicsConfig : Config::ConfigBase
	{
		RYU_DECLARE_CONFIG(GraphicsConfig, "GraphicsConfig.toml");
		RYU_DECLARE_CONFIG_SECTION(Rendering);
		RYU_DECLARE_CONFIG_SECTION(Debugging);
	
	public:
		Config::ConfigValue<bool> EnableDebugLayer        { this, DebuggingSection, "EnableDebugLayer", Common::Globals::g_isDebug };
		Config::ConfigValue<bool> EnableGPUBasedValidation{ this, DebuggingSection, "EnableGPUBasedValidation", false              };
		Config::ConfigValue<bool> EnableVSync             { this, RenderingSection, "EnableVSync", true                            };
		Config::ConfigValue<API> GraphicsAPI              { this, RenderingSection, "GraphicsAPI", API::DirectX11                  };
	};

	constexpr unsigned int FRAME_BUFFER_COUNT = 3;
}

#pragma once
#include "Common/Globals.h"
#include "Config/Config.h"
#include "GraphicsRHI/API.h"

namespace Ryu::Graphics
{
	class GraphicsConfig : Config::ConfigBase
	{
		RYU_DECLARE_CONFIG(GraphicsConfig);
		RYU_DECLARE_CONFIG_SECTION(Rendering);
		RYU_DECLARE_CONFIG_SECTION(Debugging);

	public:
		GraphicsConfig() 
			: Config::ConfigBase("GraphicsConfig.toml")
			, EnableDebugLayer(this, DebuggingSection, "EnableDebugLayer", Common::Globals::g_isDebug)
			, EnableGPUBasedValidation(this, DebuggingSection, "EnableGPUBasedValidation", false)
			, EnableVSync(this, RenderingSection, "EnableVSync", true)
			, GraphicsAPI(this, RenderingSection, "GraphicsAPI", API::DirectX11)
		{
		}
	
	public:
		Config::ConfigValue<bool> EnableDebugLayer;
		Config::ConfigValue<bool> EnableGPUBasedValidation;
		Config::ConfigValue<bool> EnableVSync;
		Config::ConfigValue<API> GraphicsAPI;
	};

	constexpr unsigned int FRAME_BUFFER_COUNT = 3;
}

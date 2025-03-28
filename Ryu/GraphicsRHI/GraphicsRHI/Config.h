#pragma once
#include "Config/ConfigManager.h"
#include "Config/ConfigBase.h"
#include "GraphicsRHI/API.h"

namespace Ryu::Graphics
{
	class GraphicsConfig : Config::ConfigBase
	{
		RYU_DECLARE_CONFIG(GraphicsConfig, "GraphicsConfig.toml")

	public:
		Config::ConfigValue<bool> EnableDebugLayer{ this, "Debugging", "EnableDebugLayer", true};
		Config::ConfigValue<bool> EnableGPUBasedValidation{ this, "Debugging", "EnableGPUBasedValidation", false};
		Config::ConfigValue<bool> EnableVSync{ this, "Rendering", "EnableVSync", false};
		Config::ConfigValue<API> GraphicsAPI{ this,  "Rendering", "GraphicsAPI", API::DirectX11 };
	
	protected:
		void Serialize(toml::table& table) const override
		{
			EnableDebugLayer.Serialize(table);
			EnableGPUBasedValidation.Serialize(table);
			EnableVSync.Serialize(table);
			GraphicsAPI.Serialize(table);
		}

		void Deserialize(const toml::table& table) override
		{
			EnableDebugLayer.Deserialize(table);
			EnableGPUBasedValidation.Deserialize(table);
			EnableVSync.Deserialize(table);
			GraphicsAPI.Deserialize(table);
		}
	};

	constexpr unsigned int FRAME_BUFFER_COUNT = 3;
}

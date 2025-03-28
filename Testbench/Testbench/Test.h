#pragma once
#include "Engine/Runtime/Runtime.h"
#include "Logger/LogCategory.h"
#include "Config/ConfigBase.h"
#include "Config/ConfigManager.h"

class TestConfig : public Ryu::Config::ConfigBase
{
	RYU_DECLARE_CONFIG(TestConfig, "TestConfig.toml")
public:
	// Display settings
	Ryu::Config::ConfigValue<std::string> WindowTitle{ this, "window", "title", "Ryu Engine" };
	Ryu::Config::ConfigValue<int> WindowWidth{ this, "window", "width", 1280 };
	Ryu::Config::ConfigValue<int> WindowHeight{ this, "window", "height", 720 };
	Ryu::Config::ConfigValue<bool> Fullscreen{ this, "window", "fullscreen", false };
	Ryu::Config::ConfigValue<bool> VSync{ this, "window", "vsync", true };

	// Graphics settings
	Ryu::Config::ConfigValue<int> MSAASamples{ this, "graphics", "msaa_samples", 4 };
	Ryu::Config::ConfigValue<float> GammaCorrection{ this, "graphics", "gamma", 2.2f };
	Ryu::Config::ConfigValue<std::string> RenderAPI{ this, "graphics", "api", "Vulkan" };

	// Performance settings
	Ryu::Config::ConfigValue<int> MaxFPS{ this, "performance", "max_fps", 144 };
	Ryu::Config::ConfigValue<bool> LimitFPS{ this, "performance", "limit_fps", true };

protected:
	void Serialize(toml::table& table) const override
	{
		WindowTitle.Serialize(table);
		WindowWidth.Serialize(table);
		WindowHeight.Serialize(table);
		Fullscreen.Serialize(table);
		VSync.Serialize(table);

		MSAASamples.Serialize(table);
		GammaCorrection.Serialize(table);
		RenderAPI.Serialize(table);

		MaxFPS.Serialize(table);
		LimitFPS.Serialize(table);
	}

	void Deserialize(const toml::table& table) override
	{
		WindowTitle.Deserialize(table);
		WindowWidth.Deserialize(table);
		WindowHeight.Deserialize(table);
		Fullscreen.Deserialize(table);
		VSync.Deserialize(table);

		MSAASamples.Deserialize(table);
		GammaCorrection.Deserialize(table);
		RenderAPI.Deserialize(table);

		MaxFPS.Deserialize(table);
		LimitFPS.Deserialize(table);
	}
};


class TestApp : public Ryu::Engine::Runtime
{
public:
	RYU_LOG_CATEGORY(TestApp);

	bool OnInit() override;
	void OnShutdown() override;
	void OnTick(f64 dt) override;	
};

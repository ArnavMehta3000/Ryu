#pragma once
#include "Common/StandardTypes.h"
#include "Config/ConfigBase.h"
#include "Config/ConfigManager.h"

namespace Ryu::App
{
	class AppConfig : Config::ConfigBase
	{
		RYU_DECLARE_CONFIG(AppConfig, "AppConfig.toml")
	public:
		Config::ConfigValue<std::string> WindowTitle{ this, "Window", "WindowTitle", "Ryu Window" };
		Config::ConfigValue<std::array<u32, 2>> WindowSize{ this, "Window", "WindowSize", { 1280, 720 } };
		Config::ConfigValue<std::array<u32, 2>> WindowMinSize{ this, "Window", "WindowMinimumSize", { 800, 600} };
		Config::ConfigValue<std::array<i32, 2>> WindowPos{ this, "Window", "WindowPosition", { static_cast<i32>(0x80000000), static_cast<i32>(0x80000000)} };
		Config::ConfigValue<bool> EscToClose{ this, "Debugging", "PressEscapeToClose", false };  // Only valid in debug builds (see Application::OnWindowKeyPressedEvent)
	
	protected:
		void Serialize(toml::table& table) const override
		{
			WindowTitle.Serialize(table);
			WindowSize.Serialize(table);
			WindowMinSize.Serialize(table);
			WindowPos.Serialize(table);
			EscToClose.Serialize(table);
		}

		void Deserialize(const toml::table& table) override
		{
			WindowTitle.Deserialize(table);
			WindowSize.Deserialize(table);
			WindowMinSize.Deserialize(table);
			WindowPos.Deserialize(table);
			EscToClose.Deserialize(table);
		}
	};
}

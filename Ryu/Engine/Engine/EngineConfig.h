#pragma once
#include "Common/Globals.h"
#include "Config/Config.h"

namespace Ryu::Engine
{
	class EngineConfig : Config::ConfigBase
	{
		RYU_DECLARE_CONFIG(EngineConfig, "EngineConfig.toml");
	};
}
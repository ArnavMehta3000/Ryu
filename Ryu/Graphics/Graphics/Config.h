#pragma once
#include "Config/Config.h"
#include "Graphics/API.h"

namespace Ryu::Graphics
{
	struct GraphicsConfig : Config::IConfig<GraphicsConfig>
	{
#if defined(RYU_BUILD_DEBUG)
		bool EnableDebugLayer{ true };
#else
		bool EnableDebugLayer{ false };
#endif
		bool EnableGPUBasedValidation{ false };  // Only used for DirectX12
		bool EnableVSync{ true };
		API GraphicsAPI{ API::DirectX12 };
	};
}

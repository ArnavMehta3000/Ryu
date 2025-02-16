#pragma once
#include "Config/Config.h"
#include "GraphicsRHI/API.h"

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

	constexpr unsigned int FRAME_BUFFER_COUNT = 3;
}

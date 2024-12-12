#pragma once
#include "Graphics/Shared/Interface/IGraphicsObject.h"
#include "Graphics/API.h"

namespace Ryu::Graphics
{
	class ISwapChain;
	struct SwapChainDesc;

	struct DeviceCreateDesc
	{
		API GraphicsAPI;
		bool EnableDebugLayer;
		bool EnableGPUBasedValidation;  // Only used for DirectX12
		bool EnableVSync;
	};

	class IDevice : public IGraphicsObject
	{
	public:
		virtual ~IDevice() = default;

		static std::unique_ptr<IDevice> Create(const DeviceCreateDesc& desc);

		virtual std::unique_ptr<ISwapChain> CreateSwapChain(const SwapChainDesc& desc) = 0;
	};
}

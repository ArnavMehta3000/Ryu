#pragma once
#include "Common/Result.h"
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
	protected:
		using CreateDeviceResult = Result<std::unique_ptr<IDevice>>;
		using CreateSwapChainResult = Result<std::unique_ptr<ISwapChain>>;

	public:
		virtual ~IDevice() = default;

		static CreateDeviceResult Create(const DeviceCreateDesc& desc);

		virtual CreateSwapChainResult CreateSwapChain(const SwapChainDesc& desc) = 0;
	};
}

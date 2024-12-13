#include "DX12Device.h"
#include "Graphics/DXGI/DXGISwapChain.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics
{
	DX12Device::DX12Device(const DeviceCreateDesc& desc)
	{
		DEBUG_ASSERT(desc.GraphicsAPI == API::DirectX12, "DeviceCreateDesc graphics API must be DirectX12!");
	}
	
	NativeObjectType DX12Device::GetNativeObject() const
	{
		return m_device;
	}
	
	IDevice::CreateSwapChainResult DX12Device::CreateSwapChain(const SwapChainDesc& desc)
	{
		return MakeResultError{ "DX12Device::CreateSwapChain method not implemented!" };
	}
}

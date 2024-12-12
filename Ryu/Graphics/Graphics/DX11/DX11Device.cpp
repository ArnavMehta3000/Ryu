#include "DX11Device.h"
#include "Graphics/DXGI/DXGISwapChain.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics
{
	DX11Device::DX11Device(const DeviceCreateDesc& desc)
	{
		DEBUG_ASSERT(desc.GraphicsAPI == API::DirectX11, "DeviceCreateDesc graphics API must be DirectX11!");
	}

	NativeObjectType DX11Device::GetNativeObject() const
	{
		return m_device;
	}
	
	IDevice::CreateSwapChainResult DX11Device::CreateSwapChain(const SwapChainDesc& desc)
	{
		return MakeResultError{ "DX11Device::CreateSwapChain method not implemented!" };
	}
}

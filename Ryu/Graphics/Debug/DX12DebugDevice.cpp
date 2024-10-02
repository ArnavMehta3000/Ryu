#include "DX12DebugDevice.h"
#include <Graphics/DX12/DX12Device.h>

namespace Ryu::Graphics
{
	DX12DebugDevice::DX12DebugDevice(const DX12Device& device)
	{
		HRESULT hr = S_OK;
		RYU_GFX_ASSERTHR(hr = device->QueryInterface(IID_PPV_ARGS(ReleaseAndGetAddressOf())), "Failed to get ID3D12DebugDevice interface");
	}
}

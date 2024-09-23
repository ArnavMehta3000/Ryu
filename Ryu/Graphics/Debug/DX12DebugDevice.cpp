#include "DX12DebugDevice.h"

namespace Ryu::Graphics
{
	DX12DebugDevice::DX12DebugDevice(ID3D12Device6* device)
	{
		HRESULT hr = S_OK;
		RYU_GFX_ASSERTHR(hr = device->QueryInterface(IID_PPV_ARGS(ReleaseAndGetAddressOf())), "Failed to get ID3D12DebugDevice interface");
	}
}

#include "DX12Debug.h"

namespace Ryu::Graphics
{
	DX12Debug::DX12Debug()
	{
		ComPtr<ID3D12Debug> debugInterface;
		HRESULT hr = S_OK;
		RYU_GFX_ASSERTHR(hr = D3D12GetDebugInterface(IID_PPV_ARGS(GetAddressOf())), "Failed to get ID3D12Debug interface");
	}
}

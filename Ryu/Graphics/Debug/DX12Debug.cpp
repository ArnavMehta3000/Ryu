#include "DX12Debug.h"

namespace Ryu::Graphics
{
	DX12Debug::DX12Debug(InterfaceType* ptr)
		: ComPtr(ptr) 
	{
	}

	CreateResult<DX12Debug> DX12Debug::Create()
	{
		DX12Debug debug;
		HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
		
		if (FAILED(hr))
		{
			return std::unexpected(hr);
		}

		return debug;
	}
}

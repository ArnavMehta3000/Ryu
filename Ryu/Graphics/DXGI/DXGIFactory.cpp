#include "DXGIFactory.h"
#include <Graphics/DXGI/DXGIAdapter.h>
#include <Graphics/Debug/DX12Debug.h>

namespace Ryu::Graphics
{
	DXGIFactory::DXGIFactory()
	{
		u32 flags = 0;

#if defined(RYU_BUILD_DEBUG)
		flags |= DXGI_CREATE_FACTORY_DEBUG;
		{
			DX12Debug debug;
			debug->EnableDebugLayer();
		}
#endif

		RYU_GFX_ASSERTHR(CreateDXGIFactory2(flags, IID_PPV_ARGS(ReleaseAndGetAddressOf())), "Failed to create DXGI factory");
	}

	DXGIAdapter DXGIFactory::GetAdapter()
	{
		HRESULT hr S_OK;

		ComPtr<IDXGIAdapter4> adapter;

		hr = Get()->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
		RYU_GFX_ASSERTHR(hr, "Failed to enumerate DXGI adapter by GPU preference (high performance)");

		return DXGIAdapter(adapter.Get());
	}

	bool DXGIFactory::HasTearingSupport()
	{
		BOOL allowTearing = FALSE;
		if (SUCCEEDED(Get()->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing))))
		{
			allowTearing = TRUE;
		}

		return allowTearing == TRUE;
	}
}

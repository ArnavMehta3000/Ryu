#include "DXGIFactory.h"
#include <Graphics/DXGI/DXGIAdapter.h>
#include <Graphics/Debug/DX12Debug.h>

namespace Ryu::Graphics
{
#define CHECK_FACTORY() if (!Get()) { RYU_GFX_ERROR("DXGIFactory is invalid"); }
	
	DXGIFactory::DXGIFactory(InterfaceType* ptr)
		: ComPtr(ptr)
	{
	}

	CreateResult<DXGIFactory::InterfaceType*> DXGIFactory::Create()
	{
		DXGIFactory::InterfaceType* outFactory;

		u32 flags = 0;

#if defined(RYU_BUILD_DEBUG)
		flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		HRESULT hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(&outFactory));
		if (FAILED(hr))
		{
			return std::unexpected(hr);
		}

		return outFactory;
	}

	void DXGIFactory::GetAdapter(DXGIAdapter& outAdapter)
	{
		CHECK_FACTORY()

		HRESULT hr S_OK;
		hr = Get()->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(outAdapter.ReleaseAndGetAddressOf()));
		RYU_GFX_ASSERTHR(hr, "Failed to enumerate DXGI adapter by GPU preference (high performance)");
	}

	bool DXGIFactory::HasTearingSupport() const
	{
		CHECK_FACTORY()

		BOOL allowTearing = FALSE;
		if (SUCCEEDED(Get()->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing))))
		{
			allowTearing = TRUE;
		}

		return allowTearing == TRUE;
	}
#undef CHECK_FACTORY
}

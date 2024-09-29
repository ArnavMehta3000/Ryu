#include "DXGIDebug.h"

namespace Ryu::Graphics
{
	DXGIDebug::DXGIDebug()
	{
		RYU_GFX_ASSERTHR(DXGIGetDebugInterface1(0, IID_PPV_ARGS(ReleaseAndGetAddressOf())), "Failed to get DXGI debug interface");
	}

	void DXGIDebug::ReportLiveObjects()
	{
		if (FAILED(Get()->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL))))
		{
			RYU_GFX_WARN("DXGI live object report failed");
		}
	}
}

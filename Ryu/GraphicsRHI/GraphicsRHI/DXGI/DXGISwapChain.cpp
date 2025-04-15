#include "DXGISwapChain.h"
#include "GraphicsRHI/GraphicsConfig.h"
#include "Profiling/Profiling.h"
#include "GraphicsRHI/Utils/Logging.h"

namespace Ryu::Graphics
{
	RYU_LOG_CATEGORY(DXGISwapChain);

	DXGISwapChain::DXGISwapChain(const SwapChainDesc& desc)
		: ISwapChain(desc)
	{
		RYU_PROFILE_SCOPE();
	}
	
	DXGISwapChain::~DXGISwapChain()
	{
		RYU_PROFILE_SCOPE();
		m_swapChain.Reset();
	}

	void DXGISwapChain::Present()
	{
		RYU_PROFILE_SCOPE();

		const bool vsync = GraphicsConfig::Get().EnableVSync;
		const bool allowTearing = m_desc.Flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		const u32 syncInterval = vsync ? 1 : 0;

		u32 presentFlags = 0;
		if (!vsync && allowTearing)
		{
			presentFlags = DXGI_PRESENT_ALLOW_TEARING;
		}
		else if (vsync && allowTearing)
		{
			// Warning: Screen tearing is requested but not allowed with vsync
			LOG_WARN(RYU_USE_LOG_CATEGORY(DXGISwapChain),
				"DXGISwapChain::Present: Tearing is requested but not allowed with VSync enabled! "
				"Disable VSync to allow tearing. Presenting with VSync enabled instead.");
		}

		DXCall(m_swapChain->Present(syncInterval, presentFlags));
	}
	
	void DXGISwapChain::Resize(u32 width, u32 height)
	{
		RYU_PROFILE_SCOPE();
	}
}

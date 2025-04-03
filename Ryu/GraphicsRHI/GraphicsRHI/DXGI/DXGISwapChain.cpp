#include "DXGISwapChain.h"
#include "GraphicsRHI/GraphicsConfig.h"
#include "Profiling/Profiling.h"
#include "GraphicsRHI/Utils/Logging.h"

namespace Ryu::Graphics
{
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
		//const GraphicsConfig& config = GraphicsConfig::Get();

		const u32 presentFlags = m_desc.Flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING ? DXGI_PRESENT_ALLOW_TEARING : 0;

		DXCall(m_swapChain->Present(0, presentFlags));
	}
	
	void DXGISwapChain::Resize(u32 width, u32 height)
	{
		RYU_PROFILE_SCOPE();
	}
}

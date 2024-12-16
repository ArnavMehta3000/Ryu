#include "DXGISwapChain.h"
#include "GraphicsRHI/Config.h"
#include "GraphicsRHI/Utils/Logging.h"

namespace Ryu::Graphics
{
	DXGISwapChain::DXGISwapChain(const SwapChainDesc& desc)
		: ISwapChain(desc)
	{
	}
	
	DXGISwapChain::~DXGISwapChain()
	{
		m_swapChain.Reset();
	}

	void DXGISwapChain::Present()
	{
		//const GraphicsConfig& config = GraphicsConfig::Get();

		const u32 presentFlags = m_desc.Flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING ? DXGI_PRESENT_ALLOW_TEARING : 0;

		DXCall(m_swapChain->Present(0, presentFlags));
	}
	
	void DXGISwapChain::Resize(u32 width, u32 height)
	{
	}
}

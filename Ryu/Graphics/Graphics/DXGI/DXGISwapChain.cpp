#include "DXGISwapChain.h"
#include "Graphics/Config.h"
#include "Graphics/Shared/Logging.h"

namespace Ryu::Graphics
{
	DXGISwapChain::DXGISwapChain(const SwapChainDesc& desc, NativeType* swapChain)
		: ISwapChain(desc)
		, m_swapChain(swapChain)
		, m_currentBackBufferIndex(0)
	{
	}
	
	DXGISwapChain::~DXGISwapChain()
	{
		m_swapChain.Reset();
	}

	void DXGISwapChain::Present()
	{
		const GraphicsConfig& config = GraphicsConfig::Get();

		const u32 presentFlags = m_desc.Flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING ? DXGI_PRESENT_ALLOW_TEARING : 0;

		DXCall(m_swapChain->Present(0, presentFlags));

		m_currentBackBufferIndex = (m_currentBackBufferIndex + 1) % m_desc.BufferCount;
	}
	
	void DXGISwapChain::Resize(u32 width, u32 height)
	{
	}
	
	u32 DXGISwapChain::GetCurrentBackBufferIndex() const
	{
		return m_swapChain->GetCurrentBackBufferIndex();
	}
	
	NativeObjectType DXGISwapChain::GetNativeObject() const
	{
		return m_swapChain.Get();
	}
	
	u32 DXGISwapChain::GetCurrentFrameIndex() const
	{
		return m_currentBackBufferIndex;
	}
}

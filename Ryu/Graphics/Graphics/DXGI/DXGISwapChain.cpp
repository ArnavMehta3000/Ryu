#include "DXGISwapChain.h"

namespace Ryu::Graphics
{
	DXGISwapChain::DXGISwapChain(NativeType* swapChain)
		: m_swapChain(swapChain)
	{
	}
	DXGISwapChain::~DXGISwapChain()
	{
		m_swapChain.Reset();
	}
	void DXGISwapChain::Present()
	{
	}
	
	void DXGISwapChain::Resize(u32 width, u32 height)
	{
	}
	
	u32 DXGISwapChain::GetCurrentBackBufferIndex() const
	{
		return 0;
	}
	
	NativeObjectType DXGISwapChain::GetNativeObject() const
	{
		return m_swapChain.Get();
	}
}

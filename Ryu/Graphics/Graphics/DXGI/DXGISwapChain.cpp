#include "DXGISwapChain.h"
#include <dxgi1_6.h>

namespace Ryu::Graphics
{
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
		return m_swapChain;
	}
}

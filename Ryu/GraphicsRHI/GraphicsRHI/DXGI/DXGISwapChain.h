#pragma once
#include "GraphicsRHI/ISwapChain.h"
#include <dxgi1_6.h>

namespace Ryu::Graphics
{
	class DXGISwapChain : public ISwapChain, public IGraphicsRHIObject<IDXGISwapChain4>
	{
	public:
		RYU_DECLARE_GFX_NATIVE_TYPE_OP(m_swapChain.Get())

		explicit DXGISwapChain(const SwapChainDesc& desc);
		virtual ~DXGISwapChain();

	protected:
		virtual void Present() override;
		virtual void Resize(u32 width, u32 height) override;

	protected:
		ComPtr<NativeType> m_swapChain;
	};
}

#pragma once
#include "Graphics/Shared/Interface/ISwapChain.h"
#include <dxgi1_6.h>

namespace Ryu::Graphics
{
	class DXGISwapChain : public ISwapChain
	{
	public:
		RYU_DECLARE_GFX_NATIVE_TYPE(IDXGISwapChain4);
		RYU_DECLARE_GFX_NATIVE_TYPE_OP(m_swapChain.Get())

		explicit DXGISwapChain(const SwapChainDesc& desc, NativeType* swapChain);
		~DXGISwapChain();

	private:
		void Present() override;
		void Resize(u32 width, u32 height) override;
		u32 GetCurrentBackBufferIndex() const override;
		NativeObjectType GetNativeObject() const override;
		u32 GetCurrentFrameIndex() const override;

	private:
		ComPtr<NativeType> m_swapChain;
		u32 m_currentBackBufferIndex = 0;
	};
}

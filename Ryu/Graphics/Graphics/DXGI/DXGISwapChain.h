#pragma once
#include "Graphics/Shared/Interface/ISwapChain.h"
#include <dxgi1_6.h>

namespace Ryu::Graphics
{
	class DXGISwapChain : public ISwapChain
	{
	public:
		RYU_DECLARE_GFX_NATIVE_TYPE(IDXGISwapChain4);

		explicit DXGISwapChain(NativeType* swapChain);
		~DXGISwapChain();

	private:
		void Present() override;
		void Resize(u32 width, u32 height) override;
		u32 GetCurrentBackBufferIndex() const override;
		NativeObjectType GetNativeObject() const override;

	private:
		ComPtr<IDXGISwapChain4> m_swapChain;
	};
}

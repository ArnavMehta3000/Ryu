#pragma once
#include "Graphics/Core/GfxTexture.h"
#include "Graphics/Core/GfxDescriptor.h"

namespace Ryu::Gfx
{
	class GfxCommandList;
	class GfxDescriptor;

	class GfxSwapChain
	{
	public:
		struct Desc
		{
			u32 Width = 0;
			u32 Height = 0;
			DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		};
	public:
		GfxSwapChain(GfxDevice* device, const GfxSwapChain::Desc& desc);
		~GfxSwapChain() = default;

		inline u32 GetBackBufferIndex() const noexcept { return m_backBufferIndex; }
		inline GfxTexture* GetBackBuffer() const noexcept { return m_backBuffers[m_backBufferIndex].get(); }

		void SetAsRenderTarget(GfxCommandList* cmdList);
		void ClearBackBuffer(GfxCommandList* cmdList);
		void Present(bool vsync);
		void OnResize(u32 width, u32 height);
	
	private:
		void CreateBackBuffers();		
		GfxDescriptor GetBackBufferDescriptor() const;

	private:
		GfxDevice*                              m_device = nullptr;
		ComPtr<DXGI::SwapChain>                 m_swapChain;
		FrameArray<std::unique_ptr<GfxTexture>> m_backBuffers;
		FrameArray<GfxDescriptor>               m_backBufferRTVs;
		u32                                     m_width;
		u32                                     m_height;
		u32                                     m_backBufferIndex;
	};
}

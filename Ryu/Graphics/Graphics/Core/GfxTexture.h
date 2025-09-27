#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	class GfxDevice;	

	class GfxTexture
	{
	public:
		struct Desc
		{
			TextureType Type                   = TextureType::Texture2D;
			u32 Width                          = 0;
			u32 Height                         = 0;
			u32 Depth                          = 0;
			u32 ArraySize                      = 1;
			u32 MipLevels                      = 1;
			u32 SampleCount                    = 1;
			HeapType HeapType                  = HeapType::Upload;
			TextureFlags BindFlags             = TextureFlags::None;
			DXGI_FORMAT Format                 = DXGI_FORMAT_UNKNOWN;
			D3D12_RESOURCE_STATES InitialState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
			CD3DX12_CLEAR_VALUE ClearValue;

			std::strong_ordering operator<=>(const GfxTexture::Desc&) const = default;
		};

	public:
		GfxTexture(GfxDevice* device, const GfxTexture::Desc& desc);
		GfxTexture(GfxDevice* device, const GfxTexture::Desc& desc, DX12::Resource* backBuffer);  // Used by swapchain
		~GfxTexture();

		RYU_GFX_NATIVE(m_resource)

		u64 GetGPUAddress() const noexcept;

	private:
		GfxDevice*             m_device = nullptr;
		ComPtr<DX12::Resource> m_resource;
		GfxTexture::Desc       m_desc;
		bool                   m_isBackBuffer = false;
	};
}

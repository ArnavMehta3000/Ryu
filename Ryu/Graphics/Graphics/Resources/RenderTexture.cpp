#include "Graphics/Resources/RenderTexture.h"
#include "Graphics/Core/Device.h"
#include "Logger/Assert.h"

namespace Ryu::Gfx
{
	RenderTexture::RenderTexture(DeviceWeakPtr parent, const TextureInitInfo& info, DescriptorHeap& rtvHeap)
		: DeviceObject(parent)
	{
		OnConstruct(info, rtvHeap);
	}
	
	RenderTexture::~RenderTexture()
	{
		OnDestruct();
	}

	void RenderTexture::OnConstruct(const TextureInitInfo& info, DescriptorHeap& rtvHeap)
	{
		if (auto parent = GetParent())
		{
			DX12::Device* const device = parent->GetDevice();

			m_texture.Initialize(parent->weak_from_this(), info, rtvHeap);
			
			m_mipCount = m_texture.GetDesc().MipLevels;			
			RYU_ASSERT(m_mipCount > Texture::MAX_MIPS, "Texture has too many mip levels");

			D3D12_RENDER_TARGET_VIEW_DESC desc{};
			desc.Format               = info.ResourceDesc->Format;
			desc.ViewDimension        = D3D12_RTV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice   = 0;
			desc.Texture2D.PlaneSlice = 0;

			for (u32 i = 0; i < m_mipCount; i++)
			{
				m_rtv[i] = rtvHeap.Allocate();

				device->CreateRenderTargetView(m_texture.Get(), &desc, m_rtv[i].CPU);
				++desc.Texture2D.MipSlice;
			}
		}
	}
	
	void RenderTexture::OnDestruct()
	{
		for (auto& rtv : m_rtv)
		{
			rtv = {};
		}
		m_mipCount = 0;
		m_texture.Destroy();
	}
}

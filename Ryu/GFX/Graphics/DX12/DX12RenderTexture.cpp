#include "DX12RenderTexture.h"
#include "Graphics/DX12/DX12Core.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics::DX12
{
	DX12Rendertexture::~DX12Rendertexture()
	{
		Release();
	}
	
	DX12Rendertexture::DX12Rendertexture(const DX12TextureCreateDesc& desc)
		: m_texture(desc)
	{
		DEBUG_ASSERT(desc.ResourceDesc);
		m_mipCount = GetResource()->GetDesc().MipLevels;
		DEBUG_ASSERT(m_mipCount && m_mipCount < DX12Texture::MAX_MIPS, "Invalid mip count");

		DescriptorHeap& rtvHeap = Core::GetRTVDescHeap();
		
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format             = desc.ResourceDesc->Format;
		rtvDesc.ViewDimension      = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		auto* const device = Core::GetDevice();
		DEBUG_ASSERT(device, "Device is invalid");

		for (auto& rtv : m_rtv)
		{
			rtv = rtvHeap.Allocate();
			device->CreateRenderTargetView(GetResource(), &rtvDesc, rtv.CPU);
			rtvDesc.Texture2D.MipSlice++;
		}
	}

	constexpr DX12Rendertexture::DX12Rendertexture(DX12Rendertexture&& other) noexcept
		: m_texture(std::move(m_texture))
		, m_mipCount(other.m_mipCount)
	{
		for (u32 i = 0; i < m_mipCount; ++i)
		{
			m_rtv[i] = other.m_rtv[i];
		}

		other.Reset();
	}

	constexpr DX12Rendertexture& DX12Rendertexture::operator=(DX12Rendertexture&& other) noexcept
	{
		DEBUG_ASSERT(this != &other);

		if (this != &other)
		{
			Release();
			Move(other);
		}

		return *this;
	}

	void DX12Rendertexture::Release()
	{
		DescriptorHeap& rtvHeap = Core::GetRTVDescHeap();
		for (auto& rtv : m_rtv)
		{
			rtvHeap.Free(rtv);
		}

		m_texture.Release();
		m_mipCount = 0;
	}

	constexpr void DX12Rendertexture::Reset()
	{
		for (auto& rtv : m_rtv)
		{
			rtv = {};
		}

		m_mipCount = 0;
	}

	constexpr void DX12Rendertexture::Move(DX12Rendertexture& other)
	{
		m_texture = std::move(other.m_texture);
		m_mipCount = other.m_mipCount;

		for (u32 i = 0; i < m_mipCount; i++)
		{
			m_rtv[i] = other.m_rtv[i];
		}

		other.Reset();
	}
}

#include "DX12DepthBuffer.h"
#include "Graphics/DX12/DX12Core.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics::DX12
{
	DX12DepthBuffer::~DX12DepthBuffer()
	{
		Release();
	}
	
	DX12DepthBuffer::DX12DepthBuffer(DX12TextureCreateDesc& desc)
	{
		DEBUG_ASSERT(desc.ResourceDesc);

		const DXGI_FORMAT dsvFormat = desc.ResourceDesc->Format;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		if (desc.ResourceDesc->Format == DXGI_FORMAT_D32_FLOAT)
		{
			desc.ResourceDesc->Format = DXGI_FORMAT_R32_TYPELESS;
			srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}

		srvDesc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels           = 1;
		srvDesc.Texture2D.MostDetailedMip     = 0;
		srvDesc.Texture2D.PlaneSlice          = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		DEBUG_ASSERT(!desc.SRVDesc && !desc.Resource);
		desc.SRVDesc = &srvDesc;

		m_texture = DX12Texture(desc);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags         = D3D12_DSV_FLAG_NONE;
		dsvDesc.Format        = dsvFormat;
		dsvDesc.Texture2D.MipSlice = 0;

		m_dsv = Core::GetDSVDescHeap().Allocate();

		auto* const device = Core::GetDevice();
		DEBUG_ASSERT(device, "Device is invalid.");

		device->CreateDepthStencilView(GetResource(), &dsvDesc, m_dsv.CPU);
	}

	constexpr DX12DepthBuffer::DX12DepthBuffer(DX12DepthBuffer&& other) noexcept
		: m_texture(std::move(other.m_texture))
		, m_dsv(other.m_dsv)
	{
	}

	constexpr DX12DepthBuffer& DX12DepthBuffer::operator=(DX12DepthBuffer&& other) noexcept
	{
		DEBUG_ASSERT(this != &other);

		if (this != &other)
		{
			m_texture = std::move(other.m_texture);
			m_dsv = other.m_dsv;
			other.m_dsv = {};
		}

		return *this;
	}
	
	void DX12DepthBuffer::Release()
	{
		Core::GetDSVDescHeap().Free(m_dsv);
		m_texture.Release();
	}
}

#include "Graphics/Resources/DepthBuffer.h"
#include "Graphics/Core/Device.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
	DepthBuffer::DepthBuffer(DeviceWeakPtr parent, TextureInitInfo& info, DescriptorHeap& dsvHeap)
		: DeviceObject(parent)
	{
		OnConstruct(info, dsvHeap);
	}

	DepthBuffer::~DepthBuffer()
	{
		OnDestruct();
	}

	void DepthBuffer::OnConstruct(TextureInitInfo& info, DescriptorHeap& dsvHeap)
	{
		RYU_ASSERT(info.ResourceDesc, "Depth buffer must have a resource description");

		if (auto parent = GetParent())
		{
			DX12::Device* const device = parent->GetDevice();

			const DXGI_FORMAT dsvFormat = info.ResourceDesc->Format;

			CD3DX12_SHADER_RESOURCE_VIEW_DESC srvDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::Tex2D(info.ResourceDesc->Format, 1);
			if (dsvFormat == DXGI_FORMAT_D32_FLOAT)
			{
				info.ResourceDesc->Format = DXGI_FORMAT_R32_TYPELESS;
				srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}

			info.SRVDesc = srvDesc;
			m_texture.Initialize(parent->weak_from_this(), info, dsvHeap);

			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.ViewDimension      = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Flags              = D3D12_DSV_FLAG_NONE;
			dsvDesc.Format             = dsvFormat;
			dsvDesc.Texture2D.MipSlice = 0;

			m_dsv = dsvHeap.Allocate();

			device->CreateDepthStencilView(m_texture.Get(), &dsvDesc, m_dsv.CPU);
		}
	}

	void DepthBuffer::OnDestruct()
	{

	}
}

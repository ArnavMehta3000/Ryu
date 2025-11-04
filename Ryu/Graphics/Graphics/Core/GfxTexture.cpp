#include "Graphics/Core/GfxTexture.h"
#include "Graphics/Core/GfxDevice.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
	GfxTexture::GfxTexture(GfxDevice* parent, DX12::Resource* resource, std::string_view name)
		: GfxDeviceChild(parent)
		, m_isBackBuffer(true)
	{
		RYU_ASSERT(resource, "Resource is null!");

		m_resource.Attach(resource);
		DX12::SetObjectName(m_resource.Get(), name.data());
	}
	
	void GfxTexture::UpdateTextureResource(DX12::Resource* resource, std::string_view name)
	{
		RYU_ASSERT(resource, "Resource is null!");

		// Release old resource
		ComRelease(m_resource);

		// Attach new resource
		m_resource.Attach(resource);
		DX12::SetObjectName(m_resource.Get(), name.data());
	}

	void GfxTexture::CreateRenderTarget(D3D12_RENDER_TARGET_VIEW_DESC* desc, GfxDescriptorHandle rtvHeapHandle)
	{
		DX12::Device* device = GetDevice()->GetNativeDevice();

		device->CreateRenderTargetView(m_resource.Get(), desc, rtvHeapHandle.CPU);
		m_isRenderTarget = true;
	}
}

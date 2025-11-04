#include "Graphics/Core/GfxTexture.h"
#include "Graphics/Core/GfxDevice.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
	Texture::Texture(Device* parent) : DeviceChild(parent) { }

	Texture::Texture(Device* parent, DX12::Resource* resource, std::string_view name)
		: DeviceChild(parent)
		, m_isBackBuffer(true)
	{
		RYU_ASSERT(resource, "Resource is null!");

		m_resource.Attach(resource);
		DX12::SetObjectName(m_resource.Get(), name.data());
	}
	
	void Texture::UpdateTextureResource(DX12::Resource* resource, std::string_view name)
	{
		RYU_ASSERT(resource, "Resource is null!");

		// Release old resource
		ComRelease(m_resource);

		// Attach new resource
		m_resource.Attach(resource);
		DX12::SetObjectName(m_resource.Get(), name.data());
	}

	void Texture::CreateRenderTarget(D3D12_RENDER_TARGET_VIEW_DESC* desc, DescriptorHandle rtvHeapHandle)
	{
		DX12::Device* device = GetDevice()->GetNativeDevice();

		device->CreateRenderTargetView(m_resource.Get(), desc, rtvHeapHandle.CPU);
		m_isRenderTarget = true;
	}
}

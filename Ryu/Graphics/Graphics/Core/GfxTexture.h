#pragma once
#include "Graphics/Core/GfxDeviceChild.h"

namespace Ryu::Gfx
{
	class Texture : public DeviceChild
	{
	public:
		Texture(Device* parent);
		Texture(Device* parent, DX12::Resource* resource, std::string_view name);  // Used when creating swapchain textures
		virtual ~Texture() = default;

		virtual inline void ReleaseObject() override { ComRelease(m_resource); }

		RYU_GFX_NATIVE(m_resource)

		void UpdateTextureResource(DX12::Resource* resource, std::string_view name); // Used when resizing swapchain textures
		void CreateRenderTarget(D3D12_RENDER_TARGET_VIEW_DESC* desc, DescriptorHandle rtvHeapHandle);
	
	private:
		ComPtr<DX12::Resource> m_resource;
		bool                   m_isBackBuffer : 1 = false;
		bool                   m_isRenderTarget : 1 = false;
	};
}

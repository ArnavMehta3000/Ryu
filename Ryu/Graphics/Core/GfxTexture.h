#pragma once
#include "Graphics/Core/GfxResource.h"

namespace Ryu::Gfx
{
	struct DescriptorHandle;

	class Texture : public Resource
	{
	public:
		Texture(Device* parent);
		Texture(Device* parent, DX12::Resource* resource, std::string_view name);  // Used when creating swapchain textures
		virtual ~Texture() = default;

		void UpdateTextureResource(DX12::Resource* resource, std::string_view name); // Used when resizing swapchain textures
		void CreateRenderTarget(D3D12_RENDER_TARGET_VIEW_DESC* desc, DescriptorHandle rtvHeapHandle);
	
	private:
		bool m_isBackBuffer : 1 = false;
		bool m_isRenderTarget : 1 = false;
	};
}

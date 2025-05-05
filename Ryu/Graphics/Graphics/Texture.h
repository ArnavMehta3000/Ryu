#pragma once
#include "Graphics/DeviceResource.h"

namespace Ryu::Gfx
{
	struct DescriptorHandle;

	struct TextureDesc
	{
		u32                 Width      = 1;
		u32                 Height     = 1;
		u32                 Mips       = 1;
		u32                 ArraySize  = 1;
		u32                 Depth      = 1;
		TextureType         Type       = TextureType::Texture2D;
		Format              Format     = Format::Unknown;
		TextureFlags        Flags      = TextureFlags::None;
		CD3DX12_CLEAR_VALUE ClearValue;
	};

	class Texture : public DeviceObject
	{
		RYU_DISABLE_COPY(Texture)
	public:
		Texture(Device* parent, const TextureDesc& desc);
		~Texture();

		inline NODISCARD DX12::Resource* GetResource() const noexcept { return m_resource.Get(); }

	private:
		ComPtr<DX12::Resource> m_resource;
		DescriptorHandle m_srv;
	};
}

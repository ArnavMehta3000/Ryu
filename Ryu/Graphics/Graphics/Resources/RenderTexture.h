#pragma once
#include "Graphics/Resources/Texture.h"
#include "Graphics/Core/DescriptorHeap.h"

namespace Ryu::Gfx
{
	class RenderTexture : public DeviceObject<RenderTexture>
	{
		RYU_GFX_DEVICE_OBJ;
	public:
		RenderTexture() = default;
		RenderTexture(DeviceWeakPtr parent, const TextureInitInfo& info, DescriptorHeap& rtvHeap);
		~RenderTexture();

		inline u32 GetMipCount() const { return m_mipCount; }
		inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetRTV(u32 mip) const { return m_rtv[mip].CPU; }
		inline DescriptorHandle GetSRV() const { return m_texture.GetSRV(); }
		inline DX12::Resource* GetResource() const { return m_texture.Get(); }

	private:
		void OnConstruct(const TextureInitInfo& info, DescriptorHeap& rtvHeap);
		void OnDestruct();

	private:
		Texture m_texture;
		std::array<DescriptorHandle, Texture::MAX_MIPS> m_rtv;
		u32 m_mipCount = 0;
	};
}

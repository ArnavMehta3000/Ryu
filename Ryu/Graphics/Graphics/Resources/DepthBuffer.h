#pragma once
#include "Graphics/Resources/Texture.h"
#include "Graphics/Core/DescriptorHeap.h"

namespace Ryu::Gfx
{
	class DepthBuffer : DeviceObject<DepthBuffer>
	{
		RYU_GFX_DEVICE_OBJ;
	public:
		DepthBuffer() = default;
		DepthBuffer(DeviceWeakPtr parent, TextureInitInfo& info, DescriptorHeap& dsvHeap);
		~DepthBuffer();

		inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetDSV(u32 mip) const { return m_dsv.CPU; }
		inline DescriptorHandle GetSRV() const { return m_texture.GetSRV(); }
		inline DX12::Resource* GetResource() const { return m_texture.Get(); }

	private:
		void OnConstruct(TextureInitInfo& info, DescriptorHeap& dsvHeap);
		void OnDestruct();
	
	private:
		Texture m_texture;
		DescriptorHandle m_dsv;
	};
}

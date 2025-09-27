#pragma once
#include "Graphics/Core/GfxDescriptor.h"

namespace Ryu::Gfx
{
	class GfxDescriptorAllocatorBase
	{
	public:
		GfxDescriptor GetHandle(u32 index = 0) const;
		[[nodiscard]] inline DX12::DescriptorHeap* GetHeap() const { return m_heap.Get(); }

	protected:
		GfxDescriptorAllocatorBase(GfxDevice* device, DescriptorHeapType type, u32 descriptorCount, bool isShaderVisible);
		void CreateHeap();

	protected:
		class GfxDevice*             m_device = nullptr;
		ComPtr<DX12::DescriptorHeap> m_heap;
		GfxDescriptor                m_headDescriptor;
		u32                          m_descriptorSize = 0;
		u32                          m_descriptorCount = 0;
		DescriptorHeapType           m_type;
		bool                         m_isShaderVisible = false;
	};
}

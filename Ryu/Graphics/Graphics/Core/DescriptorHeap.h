#pragma once
#include "Graphics/Core/DescriptorHandle.h"

namespace Ryu::Gfx
{
	// For shader-visible heaps that need GPU access  (SRV/UAV/CBV)
	class DescriptorHeap
	{
	public:
		DescriptorHeap();
		~DescriptorHeap();

		RYU_GFX_NATIVE(m_heap)

		void Create(DescriptorHeapType type, u32 maxCount);
		void Destroy();

		DescriptorHandle Allocate(u32 count = 1);
		bool ValidateHandle(const DescriptorHandle& handle) const;
		DescriptorHandle At(u32 arrayIdx) const;

		inline bool HasSpace(u32 count) const { return m_numFreeDescriptors >= count; }
		inline u32 GetDescriptorSize() const { return m_descriptorSize; }

	private:
		ComPtr<DX12::DescriptorHeap> m_heap;
		D3D12_DESCRIPTOR_HEAP_DESC   m_desc;
		u32                          m_descriptorSize;
		u32                          m_numFreeDescriptors;
		DescriptorHandle             m_firstHandle;
		DescriptorHandle             m_nextFreeHandle;
	};
}

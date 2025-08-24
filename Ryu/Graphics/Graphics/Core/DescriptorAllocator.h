#pragma once
#include "Graphics/Core/DX12.h"
#include <mutex>
#include <vector>

namespace Ryu::Gfx
{
	//For CPU-only descriptors that don't need shader visibility (RTV/DSV)
	class DescriptorAllocator
	{
	public:
		static constexpr u32 DESCRIPTORS_PER_HEAP = 256;

		DescriptorAllocator(DescriptorHeapType type);

		CD3DX12_CPU_DESCRIPTOR_HANDLE Allocate(u32 count);
		static void DestroyAll();
	
	private:
		static DX12::DescriptorHeap* RequestNewHeap(DescriptorHeapType type);

	private:
		DescriptorHeapType            m_type;
		DX12::DescriptorHeap*         m_currentHeap;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_currentHandle;
		u32                           m_descriptorSize;
		u32                           m_remainingFreeHandles;

		static std::mutex s_allocationMutex;
		static std::vector<ComPtr<DX12::DescriptorHeap>> s_heapPool;
	};
}

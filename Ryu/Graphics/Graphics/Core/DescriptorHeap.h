#pragma once
#include "Graphics/Core/DeviceObject.h"
#include <mutex>

namespace Ryu::Gfx
{
	class DescriptorHeap;

	struct DescriptorHandle
	{
		DescriptorHandle()
			: CPUHandle(InvalidCPUHandle)
			, GPUHandle(InvalidGPUHandle)
			, HeapIndex(InvalidHeapIndex)
		{}

		DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, u32 heapIndex, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = InvalidGPUHandle)
			: CPUHandle(cpuHandle)
			, GPUHandle(gpuHandle)
			, HeapIndex(heapIndex)
		{}

		void OffsetInline(const u32 numDescriptors, const u32 descriptorSize)
		{
			if (CPUHandle != InvalidCPUHandle) { CPUHandle.Offset(numDescriptors, descriptorSize); }			
			if (GPUHandle != InvalidGPUHandle) { GPUHandle.Offset(numDescriptors, descriptorSize); }			
			if (HeapIndex != InvalidHeapIndex) { HeapIndex += numDescriptors; }
		}
		
		DescriptorHandle Offset(const u32 numDescriptors, const u32 descriptorSize)
		{
			DescriptorHandle handle = *this;
			handle.OffsetInline(numDescriptors, descriptorSize);
			return handle;
		}

		void Reset()
		{
			CPUHandle = InvalidCPUHandle;
			GPUHandle = InvalidGPUHandle;
			HeapIndex = InvalidHeapIndex;
		}

		bool IsValid() const { return CPUHandle.ptr != 0; }
		bool IsShaderVisible() const { return GPUHandle.ptr != 0; }

		constexpr static u32 InvalidHeapIndex                         = 0xFFFFFFFF;
		constexpr static D3D12_CPU_DESCRIPTOR_HANDLE InvalidCPUHandle = { ~0u };
		constexpr static D3D12_GPU_DESCRIPTOR_HANDLE InvalidGPUHandle = { ~0u };

		CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandle;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandle;
		u32 HeapIndex;
		DescriptorHeap* OwnerHeap = nullptr;
	};

	class DescriptorHeap : public DeviceObject
	{
		RYU_LOG_DECLARE_CATEGORY(DescriptorHeap);

	public:
		DescriptorHeap(std::weak_ptr<Device> parent, DescHeapType type, DescHeapFlags flags, u32 numDescriptors);
		~DescriptorHeap();

		inline NODISCARD DX12::DescHeap* GetHeap() const { return m_heap.Get(); }

		NODISCARD DescriptorHandle Allocate();
		void Free(DescriptorHandle& handle);

	private:
		ComPtr<DX12::DescHeap>        m_heap;
		FreeList                      m_freeList;
		std::mutex                    m_mutex;
		u32                           m_numDescriptors;
		u32                           m_descriptorSize;
		DescHeapType                  m_type;
	};
}

#pragma once
#include "Graphics/DeviceResource.h"
#include <mutex>

namespace Ryu::Gfx
{
	class DescriptorHeap : public DeviceObject
	{
		RYU_LOG_DECLARE_CATEGORY(DescriptorHeap);
	public:
		struct Handle
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandle;
			CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandle;

			constexpr bool IsValid() const { return CPUHandle.ptr != 0; }
			// Only shader visible heaps have GPU handles
			constexpr bool IsShaderVisible() const { return GPUHandle.ptr != 0; }

#if defined(RYU_BUILD_DEBUG)
		private:
			friend class DescriptorHeap;
			DescriptorHeap* m_ownerHeap = nullptr;
			u32             m_index = 0;
#endif
		};

	public:
		DescriptorHeap(Device* parent, DescHeapType type);
		~DescriptorHeap();

		inline NODISCARD DX12::DescHeap* GetHeap() const noexcept{ return m_heap.Get(); }
		inline NODISCARD CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const noexcept { return m_cpuHandle; }
		inline NODISCARD CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const noexcept { return m_gpuHandle; }
		inline NODISCARD u32 GetCapacity() const noexcept { return m_capacity; }
		inline NODISCARD u32 GetSize() const noexcept { return m_size; }
		inline NODISCARD u32 GetDescriptorSize() const noexcept { return m_descriptorSize; }
		inline NODISCARD DescHeapType GetType() const noexcept { return m_type; }
		inline NODISCARD bool IsShaderVisible() const noexcept { return m_gpuHandle.ptr != 0; }

		bool Init(u32 capacity, DescHeapFlags flags);
		void ReleaseHeap();
		NODISCARD Handle Allocate();
		void Free(Handle& handle);

		void ProcessDeferredFree(const u32 frameIndex);

	private:
		ComPtr<DX12::DescHeap>        m_heap;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
		CD3DX12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;
		std::unique_ptr<u32[]>        m_freeHandles;  // Size same as capacity
		std::vector<u32>              m_deferredFreeIndices[FRAME_BUFFER_COUNT];
		std::mutex                    m_mutex;
		u32                           m_capacity;
		u32                           m_size;
		u32                           m_descriptorSize;
		DescHeapType                  m_type;
	};
}

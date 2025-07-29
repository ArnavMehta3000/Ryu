#pragma once
#include "Graphics/Core/DeviceObject.h"

namespace Ryu::Gfx
{
	struct DescriptorHandle
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE CPU;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPU;

		bool IsValid() const { return CPU.ptr != 0; }
		bool IsShaderVisible() const { return GPU.ptr != 0; }

#if defined(RYU_BUILD_DEBUG)
		friend class DescriptorHeap;
		DescriptorHeap* m_container{ nullptr };
		u32 m_index{ 0u };
#endif
	};


	class DescriptorHeap : public DeviceObject<DescriptorHeap>
	{
		RYU_DISABLE_COPY_AND_MOVE(DescriptorHeap)
		RYU_GFX_DEVICE_OBJ;
	public:
		RYU_GFX_NATIVE(m_heap)
		DescriptorHeap() = default;
		DescriptorHeap(std::weak_ptr<Device> parent, DescriptorHeapType type);
		~DescriptorHeap() = default;

		inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUStart() const { return m_cpuStart; }
		inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUStart() const { return m_gpuStart; }
		inline DescriptorHeapType GetType() const { return m_type; }
		inline u32 GetSize() const { return m_size; }
		inline u32 GetDescriptorSize() const { return m_descriptorSize; }
		inline u32 GetCapacity() const { return m_capacity; }
		inline bool IsShaderVisible() const { return m_gpuStart.ptr != 0; }

		bool Initialize(u32 capacity, bool isShaderVisible);
		void Release();
		DescriptorHandle Allocate();
		void Free(DescriptorHandle& handle);
		void ProcessDeferredFree(u32 frameIndex);

	private:
		void OnConstruct(DescriptorHeapType type);

	private:
		ComPtr<DX12::DescriptorHeap>  m_heap;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_cpuStart;
		CD3DX12_GPU_DESCRIPTOR_HANDLE m_gpuStart;
		std::unique_ptr<u32[]>        m_freeHandles;
		FrameArray<std::vector<u32>>  m_deferredFreeHandles;
		u32                           m_capacity{ 0 };
		u32                           m_size{ 0 };
		u32                           m_descriptorSize{ 0 };
		DescriptorHeapType            m_type;
	};
}

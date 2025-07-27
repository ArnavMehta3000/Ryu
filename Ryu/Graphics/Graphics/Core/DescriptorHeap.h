#pragma once
#include "Graphics/Core/DeviceObject.h"

namespace Ryu::Gfx
{
	class DescriptorHeap final: public DeviceObject<DescriptorHeap>
	{
		RYU_GFX_DEVICE_OBJ;
	public:
		RYU_GFX_NATIVE(m_heap)

		constexpr static u32 InvalidHeapIndex = 0xFFFFFFFF;
		constexpr static D3D12_CPU_DESCRIPTOR_HANDLE InvalidCPUHandle = { ~0u };
		constexpr static D3D12_GPU_DESCRIPTOR_HANDLE InvalidGPUHandle = { ~0u };

		DescriptorHeap() = default;
		DescriptorHeap(std::weak_ptr<Device> parent, DescriptorHeapType type, DescriptorHeapFlags flags, u32 numDescriptors);
		~DescriptorHeap();

		inline NODISCARD CD3DX12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle() { return m_cpuHandle; }
		inline NODISCARD CD3DX12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() { return m_gpuHandle; }
		inline NODISCARD u32 GetDescriptorSize() const { return m_descriptorSize; }

		bool IsValid() const;
		bool IsShaderVisible() const;

	private:
		void OnConstruct(DescriptorHeapType type, DescriptorHeapFlags flags, u32 numDescriptors);
		void OnDestruct();
	
	private:
		ComPtr<DX12::DescriptorHeap>  m_heap;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
		CD3DX12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;
		DescriptorHeapType            m_type;
		u32                           m_count;
		u32                           m_descriptorSize;
	};

	class DescHeap;

	struct DescriptorHandle
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE CPU;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPU;

		bool IsValid() const { return CPU.ptr != 0; }
		bool IsShaderVisible() const { return GPU.ptr != 0; }

#if defined(RYU_BUILD_DEBUG)
		friend class DescHeap;
		DescHeap* m_container{ nullptr };
		u32 m_index{ 0u };
#endif
	};


	class DescHeap : public DeviceObject<DescHeap>
	{
		RYU_DISABLE_COPY_AND_MOVE(DescHeap)
		RYU_GFX_DEVICE_OBJ;
	public:
		RYU_GFX_NATIVE(m_heap)
			DescHeap() = default;
		DescHeap(std::weak_ptr<Device> parent, DescriptorHeapType type);
		~DescHeap();

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

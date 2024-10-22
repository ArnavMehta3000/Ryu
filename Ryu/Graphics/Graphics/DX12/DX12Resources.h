#pragma once
#include "Graphics/Common.h"
#include <mutex>
#include <vector>

namespace Ryu::Graphics::DX12
{
	class DescriptorHeap;

	struct DescriptorHandle
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE CPU{};
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPU{};

		constexpr inline bool IsValid() const noexcept { return CPU.ptr != 0; }
		constexpr inline bool IsShaderVisible() const noexcept { return GPU.ptr != 0; }

	private:
#if defined(RYU_BUILD_DEBUG)
		friend class DescriptorHeap;
		DescriptorHeap* Container{ nullptr };
		u32 Index{ 0 };
#endif
	};

	// Representa a chunk of memory on the CPU/GPU
	class DescriptorHeap
	{
	public:
		RYU_DISABLE_COPY_AND_MOVE(DescriptorHeap);

		explicit DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);
		~DescriptorHeap();

		bool Init(u32 capacity, bool isShaderVisible);
		void Release();
		NODISCARD DescriptorHandle Allocate();
		void Free(DescriptorHandle& handle);
		void ProcessDeferredFree(u32 frameIndex);

		constexpr inline ID3D12DescriptorHeap* const GetHeap() const noexcept { return m_heap; }
		constexpr inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuStart() const noexcept { return m_cpuStart; }
		constexpr inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuStart() const noexcept { return m_gpuStart; }
		constexpr inline u32 GetDescriptorSize() const noexcept { return m_descriptorSize; }
		constexpr inline u32 GetCapacity() const noexcept { return m_capacity; }
		constexpr inline u32 GetSize() const noexcept { return m_size; }
		constexpr inline D3D12_DESCRIPTOR_HEAP_TYPE GetType() const noexcept { return m_type; }
		constexpr inline bool IsShaderVisible() const noexcept { return m_gpuStart.ptr != 0; }

	private:
		ID3D12DescriptorHeap*            m_heap;
		CD3DX12_CPU_DESCRIPTOR_HANDLE    m_cpuStart;
		CD3DX12_GPU_DESCRIPTOR_HANDLE    m_gpuStart;
		std::unique_ptr<u32[]>           m_freeHandles;
		std::vector<u32>                 m_deferredFreeIndices[FRAME_BUFFER_COUNT];
		std::mutex                       m_mutex;
		u32                              m_capacity{ 0 };
		u32                              m_size{ 0 };
		u32                              m_descriptorSize{ 0 };
		const D3D12_DESCRIPTOR_HEAP_TYPE m_type;
	};
}

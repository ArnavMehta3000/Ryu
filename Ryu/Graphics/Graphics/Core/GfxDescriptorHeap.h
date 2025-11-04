#pragma once
#include "Graphics/Core/GfxDeviceChild.h"
#include <queue>

namespace Ryu::Gfx
{
	struct DescriptorHandle
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE CPU = CD3DX12_DEFAULT{};
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPU = CD3DX12_DEFAULT{};
		u32 Index = UINT32_MAX;

		[[nodiscard]] inline bool IsValid() const { return Index != UINT32_MAX; }
		[[nodiscard]] inline bool IsShaderVisible() const { return GPU.ptr != 0; }
	};

	class DescriptorHeap : public DeviceChild
	{
	public:
		DescriptorHeap(Device* parent, D3D12_DESCRIPTOR_HEAP_TYPE type,
			u32 numDescriptors, bool isShaderVisible, std::string_view name);
		~DescriptorHeap() override = default;

		inline virtual void ReleaseObject() override { ComRelease(m_heap); }
		
		RYU_DISABLE_COPY(DescriptorHeap)
		RYU_GFX_NATIVE(m_heap)

		[[nodiscard]] DescriptorHandle Allocate();
		void Free(const DescriptorHandle& handle);
		void Reset();  // Free all descriptors

		[[nodiscard]] DescriptorHandle GetHandle(u32 index) const;
		[[nodiscard]] inline u32 GetDescriptorSize() const { return m_descriptorSize; }

	private:
		ComPtr<DX12::DescriptorHeap>  m_heap;
		D3D12_DESCRIPTOR_HEAP_TYPE    m_type;
		u32                           m_descriptorSize;
		u32                           m_numDescriptors;
		bool                          m_isShaderVisible;

		CD3DX12_CPU_DESCRIPTOR_HANDLE m_cpuStart;
		CD3DX12_GPU_DESCRIPTOR_HANDLE m_gpuStart;

		std::queue<u32>               m_freeIndices;
		u32                           m_nextIndex;
	};
}

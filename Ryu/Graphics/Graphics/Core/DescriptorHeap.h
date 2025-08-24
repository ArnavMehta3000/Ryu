#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	struct DescriptorHandle
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE CPU;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPU;

		DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu) : CPU(cpu), GPU(gpu) {}
		DescriptorHandle() : CPU(D3D12_DEFAULT), GPU(D3D12_DEFAULT) {}

		void Offset(u32 count, u32 incrementSize) { CPU.Offset(count, incrementSize); GPU.Offset(count, incrementSize); }

		operator CD3DX12_CPU_DESCRIPTOR_HANDLE() const { return CPU; }
		operator CD3DX12_GPU_DESCRIPTOR_HANDLE() const { return GPU; }

		bool IsValid() const { return CPU.ptr != 0; }
		bool IsShaderVisible() const { return GPU.ptr != 0; }
	};

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

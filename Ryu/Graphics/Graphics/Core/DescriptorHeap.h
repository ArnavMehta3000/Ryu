#pragma once
#include "Graphics/Core/DeviceObject.h"

namespace Ryu::Gfx
{
	class DescriptorHeap final: public DeviceObject<DescriptorHeap>
	{
		RYU_GFX_DEVICE_OBJ;
	public:
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
}

#include "Graphics/Core/GfxDescriptorHeap.h"
#include "Graphics/Core/GfxDevice.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
	DescriptorHeap::DescriptorHeap(
		Device* parent, D3D12_DESCRIPTOR_HEAP_TYPE type, 
		u32 numDescriptors, bool isShaderVisible, std::string_view name)
		: DeviceChild(parent)
		, m_type(type)
		, m_numDescriptors(numDescriptors)
		, m_isShaderVisible(isShaderVisible)
		, m_nextIndex(0)
	{
		RYU_ASSERT(isShaderVisible
			? numDescriptors <= D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2
			: numDescriptors <= D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE, "Descriptor heap is too large!");

		D3D12_DESCRIPTOR_HEAP_DESC desc
		{
			.Type           = type,
			.NumDescriptors = numDescriptors,
			.Flags          = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE
		};

		DX12::Device* device = GetDevice()->GetNativeDevice();

		DXCall(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap)));
		DX12::SetObjectName(m_heap.Get(), name.data());

		m_descriptorSize = device->GetDescriptorHandleIncrementSize(type);
		m_cpuStart = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_heap->GetCPUDescriptorHandleForHeapStart());

		if (isShaderVisible)
		{
			m_gpuStart = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_heap->GetGPUDescriptorHandleForHeapStart());
		}
	}
	
	DescriptorHandle DescriptorHeap::Allocate()
	{
		u32 index{};

		if (!m_freeIndices.empty())
		{
			index = m_freeIndices.front();
			m_freeIndices.pop();
		}
		else if (m_nextIndex < m_numDescriptors)
		{
			index = m_nextIndex++;
		}
		else
		{
			// Heap is full
			RYU_LOG_ERROR("Descriptor heap is full!");
			return DescriptorHandle{};
		}

		return GetHandle(index);
	}
	
	void DescriptorHeap::Free(const DescriptorHandle& handle)
	{
		if (handle.IsValid())
		{
			m_freeIndices.push(handle.Index);
		}
	}
	
	void DescriptorHeap::Reset()
	{
		m_nextIndex = 0;
		m_freeIndices = std::queue<u32>();
	}

	u32 DescriptorHeap::FindIndex(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle) const
	{
		u64 offset = cpuHandle.ptr - GetHandle(0).CPU.ptr;
		return u32(offset / m_descriptorSize);
	}
	
	DescriptorHandle DescriptorHeap::GetHandle(u32 index) const
	{
		DescriptorHandle handle;
		handle.Index = index;
		handle.CPU   = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cpuStart, index, m_descriptorSize);

		if (m_isShaderVisible)
		{
			handle.GPU = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_gpuStart, index, m_descriptorSize);
		}

		return handle;
	}
}

#include "Graphics/Core/GfxDescriptorAllocatorBase.h"
#include "Graphics/Core/GfxDevice.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
	GfxDescriptor GfxDescriptorAllocatorBase::GetHandle(u32 index) const
	{
		RYU_ASSERT(m_heap, "Descriptor heap must be created before getting handles.");
		RYU_ASSERT(index < m_descriptorCount, "Descriptor index out of range.");

		GfxDescriptor handle = m_headDescriptor;
		handle.Increment(m_descriptorSize, index);
		return handle;
	}
	
	GfxDescriptorAllocatorBase::GfxDescriptorAllocatorBase(GfxDevice* device, DescriptorHeapType type, u32 descriptorCount, bool isShaderVisible)
		: m_device(device)
		, m_type(type)
		, m_descriptorCount(descriptorCount)
		, m_isShaderVisible(isShaderVisible)
	{
		CreateHeap();
		
		m_headDescriptor.m_cpu = m_heap->GetCPUDescriptorHandleForHeapStart();
		if (m_isShaderVisible)
		{
			m_headDescriptor.m_gpu = m_heap->GetGPUDescriptorHandleForHeapStart();
		}
		m_headDescriptor.m_index = 0;
	}
	
	void GfxDescriptorAllocatorBase::CreateHeap()
	{
		RYU_ASSERT(m_descriptorCount <= UINT32_MAX, "Too many descriptors for a single heap!");
		
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
		heapDesc.Type           = DX12::ToNative(m_type);
		heapDesc.Flags          = m_isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDesc.NodeMask       = 0;
		heapDesc.NumDescriptors = m_descriptorCount;
		
		DX12::Device* dx12Device = m_device->GetDevice();
		DXCallEx(m_device->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_heap)), dx12Device);
		m_descriptorSize = dx12Device->GetDescriptorHandleIncrementSize(heapDesc.Type);
	}
}

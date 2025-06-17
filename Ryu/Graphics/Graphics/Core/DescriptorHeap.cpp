#include "Graphics/Core/DescriptorHeap.h"
#include "Graphics/Core/Device.h"
#include "Logger/Logger.h"
#include <scoped_allocator>

namespace Ryu::Gfx
{
	DescriptorHeap::DescriptorHeap(std::weak_ptr<Device> parent, DescHeapType type, DescHeapFlags flags, u32 numDescriptors)
		: DeviceObject(parent)
		, m_numDescriptors(numDescriptors)
		, m_freeList(numDescriptors)
		, m_descriptorSize(0)
		, m_type(type)
	{
		bool isShaderVisible = flags == DescHeapFlags::ShaderVisible;
		if ((m_type == DescHeapType::DSV || m_type == DescHeapType::RTV) && isShaderVisible)
		{
			// Remove shader visible flag even if it was passed in
			RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(DescriptorHeap),
				"Removing shader visible flag from descriptor heap as it is not supported for RTV/DSV");

			flags = DescHeapFlags::None;  // Can hard code since there are only 2 flags
			isShaderVisible = false;
		}

		if (auto parentDevice = GetParent())
		{
			DX12::Device* const device = parentDevice->GetDevice();

			D3D12_DESCRIPTOR_HEAP_DESC desc{};
			desc.NumDescriptors = FRAME_BUFFER_COUNT;
			desc.Type           = DX12::GetDescHeapType(m_type);
			desc.Flags          = static_cast<D3D12_DESCRIPTOR_HEAP_FLAGS>(flags);
			desc.NodeMask       = 0;
			DXCallEx(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap)), device);
			DX12::SetObjectName(m_heap.Get(), std::format("{} Descriptor Heap", Internal::DescHeapTypeToString(desc.Type)).c_str());

			m_descriptorSize = device->GetDescriptorHandleIncrementSize(desc.Type);
		}
		else
		{
			RYU_LOG_ERROR(RYU_LOG_USE_CATEGORY(DescriptorHeap), "Parent device is null");
		}
	}
	
	DescriptorHeap::~DescriptorHeap()
	{
		if (m_heap)
		{
			if (auto parent = GetParent())
			{
				parent->DeferReleaseObject(m_heap.Detach());
			}
		}
	}

	DescriptorHandle DescriptorHeap::Allocate()
	{
		std::lock_guard lock(m_mutex);
		
		if (!m_freeList.CanAllocate())
		{
			RYU_LOG_ERROR(RYU_LOG_USE_CATEGORY(DescriptorHeap), "Descriptor heap is full, cannot allocate!");
			return DescriptorHandle();
		}

		u32 index = m_freeList.Allocate();

		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(m_heap->GetCPUDescriptorHandleForHeapStart());
		cpuHandle.Offset(index, m_descriptorSize);

		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(DescriptorHandle::InvalidGPUHandle);
		if (m_heap->GetDesc().Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
		{
			gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_heap->GetGPUDescriptorHandleForHeapStart());
			gpuHandle.Offset(index, m_descriptorSize);
		}

		DescriptorHandle handle(cpuHandle, index, gpuHandle);

		RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(DescriptorHeap), "Allocated descriptor: Index={}, Type={}",
			index, Internal::DescHeapTypeToString(DX12::GetDescHeapType(m_type)));

		return handle;
	}
	
	void DescriptorHeap::Free(DescriptorHandle& handle)
	{
		if (!handle.IsValid())
		{
			RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(DescriptorHeap), "Attempted to free invalid descriptor handle");
			return;
		}

		std::lock_guard lock(m_mutex);
		const u32 index = handle.HeapIndex;
		m_freeList.Free(index);

		RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(DescriptorHeap), "Freed descriptor: Index={}, Type={}",
			index, Internal::DescHeapTypeToString(DX12::GetDescHeapType(m_type)));

		handle.Reset();
	}
}

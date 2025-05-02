#include "Graphics/DescriptorHeap.h"
#include "Graphics/Device.h"
#include "Logger/Logger.h"
#include <libassert/assert.hpp>

namespace Ryu::Gfx
{
	DescriptorHeap::DescriptorHeap(Device* parent, DescHeapType type)
		: DeviceObject(parent)
		, m_capacity(0)
		, m_size(0)
		, m_descriptorSize(0)
		, m_type(type)
	{
	}
	
	DescriptorHeap::~DescriptorHeap()
	{
		ReleaseHeap();
	}
	
	bool DescriptorHeap::Init(u32 capacity, DescHeapFlags flags)
	{
		if (capacity && capacity > D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2)
		{
			RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(DX12),
				"Descriptor heap input capacity ({}) must be less than D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2({})",
				capacity, D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2);
			return false;
		}

		if (m_type == DescHeapType::Sampler && capacity > D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE)
		{
			RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(DX12),
				"Descriptor heap input capacity ({}) must be less than D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE({})",
				capacity, D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE);
			return false;
		}

		bool isShaderVisible = flags == DescHeapFlags::ShaderVisible;
		if ((m_type == DescHeapType::DSV || m_type == DescHeapType::RTV) && isShaderVisible)
		{
			// Remove shader visible flag even if it was passed in
			RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(DescriptorHeap),
				"Removing shader visible flag from descriptor heap as it is not supported for RTV/DSV");

			flags = DescHeapFlags::None;  // Can hard code since there are only 2 flags
			isShaderVisible = false;
		}

		m_capacity = capacity;

		ReleaseHeap();

		DX12::Device* const device = GetParent()->GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.NumDescriptors = FRAME_BUFFER_COUNT;
		desc.Type           = DX12::GetDescHeapType(m_type);
		desc.Flags          = static_cast<D3D12_DESCRIPTOR_HEAP_FLAGS>(flags);
		desc.NodeMask       = 0;
		DXCallEx(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap)), device);
		DX12::SetObjectName(m_heap.Get(), std::format("{} Descriptor Heap", Internal::DescHeapTypeToString(desc.Type)).c_str());

		// Create free handles
		m_freeHandles = std::make_unique<u32[]>(m_capacity);
		for (u32 i = 0; i < m_capacity; i++)
		{
			m_freeHandles[i] = i;
		}
		
#if defined(RYU_BUILD_DEBUG)
		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			DEBUG_ASSERT(m_deferredFreeIndices[i].empty());
		}
#endif

		// Get sizes and handles
		m_descriptorSize = device->GetDescriptorHandleIncrementSize(desc.Type);
		m_cpuHandle = m_heap->GetCPUDescriptorHandleForHeapStart();
		m_gpuHandle = isShaderVisible ? m_heap->GetGPUDescriptorHandleForHeapStart() : CD3DX12_GPU_DESCRIPTOR_HANDLE();

		return true;
	}

	void DescriptorHeap::ReleaseHeap()
	{
		m_freeHandles.reset();
		m_heap.Reset();
	}

	DescriptorHeap::Handle DescriptorHeap::Allocate()
	{
		std::lock_guard lock(m_mutex);

		ASSERT(m_size < m_capacity, "Descriptor heap is full");

		const u32 index = m_freeHandles[m_size++];
		const u32 offset = index * m_descriptorSize;
		++m_size;

		Handle handle;
		handle.CPUHandle.InitOffsetted(m_cpuHandle, offset);
		if (IsShaderVisible())
		{
			handle.GPUHandle.InitOffsetted(m_gpuHandle, offset);
		}

#if defined(RYU_BUILD_DEBUG)
		handle.m_ownerHeap = this;
		handle.m_index     = index;
#endif

		return handle;
	}
	
	void DescriptorHeap::Free(Handle& handle)
	{
		if (!handle.IsValid())
		{
			return;
		}

		if (m_heap && m_size == 0)
		{
			RYU_LOG_ERROR(RYU_LOG_USE_CATEGORY(DX12), "Trying to free from an empty descriptor heap!");
			return;
		}

		std::lock_guard lock(m_mutex);

		DEBUG_ASSERT(handle.m_ownerHeap == this, "Descriptor heap handle does not match this heap");
		DEBUG_ASSERT(handle.CPUHandle.ptr >= m_cpuHandle.ptr, "Descriptor heap handle does not match this heap");
		DEBUG_ASSERT((handle.CPUHandle.ptr - m_cpuHandle.ptr) % m_descriptorSize == 0, "Descriptor heap handle does not match this heap");
		DEBUG_ASSERT(handle.m_index < m_capacity);

		const u32 index = static_cast<u32>(handle.CPUHandle.ptr - m_cpuHandle.ptr) / m_descriptorSize;
		DEBUG_ASSERT(handle.m_index == index, "Descriptor heap handle does not match this heap");

		const u32 frameIndex = GetParent()->GetCommandContext()->GetFrameIndex();
		m_deferredFreeIndices[frameIndex].push_back(index);
		handle = {};
	}

	void DescriptorHeap::ProcessDeferredFree(const u32 frameIndex)
	{
		std::lock_guard lock(m_mutex);

		std::span<u32> indices{ m_deferredFreeIndices[frameIndex] };
		if (!indices.empty())
		{
			for (auto index : indices)
			{
				--m_size;
				m_freeHandles[m_size] = index;
			}
		}
	}
}

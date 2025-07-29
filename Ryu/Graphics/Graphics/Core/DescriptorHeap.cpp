#include "Graphics/Core/DescriptorHeap.h"
#include "Graphics/Core/Device.h"
#include "Profiling/Profiling.h"
#include "Logger/Assert.h"
#include <format>

namespace Ryu::Gfx
{
	// TODO: Use a std:mutex to lock access to the heap

	DescriptorHeap::DescriptorHeap(std::weak_ptr<Device> parent, DescriptorHeapType type)
		: DeviceObject(parent)
		, m_type(type)
	{
	}

	bool DescriptorHeap::Initialize(u32 capacity, bool isShaderVisible)
	{
		RYU_ASSERT(capacity && capacity < D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2,
			"Descriptor heap capacity must be between 1 and D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2.");
		
		RYU_ASSERT(!(m_type == DescriptorHeapType::Sampler && capacity < D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE),
			"Sampler heap capacity must be between 1 and D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE.");

		if (m_type == DescriptorHeapType::DSV || m_type == DescriptorHeapType::RTV)
		{
			isShaderVisible = false;
		}

		Release();

		if (auto parent = GetParent())
		{
			if (DX12::Device* const device = parent->GetDevice())
			{
				D3D12_DESCRIPTOR_HEAP_DESC desc
				{
					.Type           = DX12::ToNative(m_type),
					.NumDescriptors = capacity,
					.Flags          = DX12::ToNative(isShaderVisible ? DescriptorHeapFlags::ShaderVisible : DescriptorHeapFlags::None),
					.NodeMask       = 0
				};

				DXCallEx(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap)), device);

				DX12::SetObjectName(m_heap.Get(),
					std::format("[{}] Descriptor Heap ({})",
						isShaderVisible ? "Shader Visible" : "Non-Shader Visible",
						EnumToString(m_type)).c_str());

				m_freeHandles = std::move(std::make_unique<u32[]>(capacity));
				m_capacity = capacity;
				m_size = 0;
				
				for (u32 i = 0; i < capacity; ++i)
				{
					m_freeHandles[i] = i;
				}
				
#if defined(RYU_BUILD_DEBUG)
				for (u32 i = 0; i < m_deferredFreeHandles.size(); ++i)
				{
					RYU_ASSERT(m_deferredFreeHandles[i].empty());
				}
#endif

				m_descriptorSize = device->GetDescriptorHandleIncrementSize(desc.Type);
				m_cpuStart       = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_heap->GetCPUDescriptorHandleForHeapStart());
				m_gpuStart       = isShaderVisible
					? CD3DX12_GPU_DESCRIPTOR_HANDLE(m_heap->GetGPUDescriptorHandleForHeapStart())
					: D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };

				return true;
			}
		}

		return false;
	}

	void DescriptorHeap::Release()
	{
		RYU_ASSERT(!m_size);
		if (auto parent = GetParent())
		{
			// This will make seld nullptr and hand over the resource to the device to handle
			parent->DeferredRelease(m_heap.Detach());
		}
	}

	DescriptorHandle DescriptorHeap::Allocate()
	{
		RYU_ASSERT(m_size < m_capacity, "Descriptor heap is full.");
		RYU_ASSERT(m_heap, "Descriptor heap is not initialized.");

		const u32 index = m_freeHandles[m_size];

		DescriptorHandle handle;
		handle.CPU = m_cpuStart;
		handle.CPU.Offset(m_freeHandles[m_size], m_descriptorSize);
		if (IsShaderVisible())
		{
			handle.GPU = m_gpuStart;
			handle.GPU.Offset(m_freeHandles[m_size], m_descriptorSize);
		}

		++m_size;

#if defined(RYU_BUILD_DEBUG)
		handle.m_container = this;
		handle.m_index     = index;
#endif

		return handle;
	}

	void DescriptorHeap::Free(DescriptorHandle& handle)
	{
		if (!handle.IsValid())
		{
			return;
		}

		RYU_ASSERT(m_heap, "Descriptor heap is not initialized.");
		RYU_ASSERT(m_size > 0, "Descriptor heap is empty.");
		RYU_ASSERT(handle.CPU.ptr >= m_cpuStart.ptr, "Descriptor CPU handle is out of range.");
		RYU_ASSERT((handle.CPU.ptr - m_cpuStart.ptr) % m_descriptorSize == 0, "Descriptor CPU handle is not aligned.");
		RYU_ASSERT(handle.m_index < m_capacity, "Descriptor index is out of range.");

		const u32 index = (u32)((handle.CPU.ptr - m_cpuStart.ptr) / m_descriptorSize);
		RYU_ASSERT(handle.m_index == index, "Descriptor index mismatch.");

		if (auto parent = GetParent())
		{
			const u32 frameIndex = parent->GetCurrentFrameIndex();
			m_deferredFreeHandles[frameIndex].push_back(index);
			parent->SetDeferredReleaseFlag();
		}

		handle = {};
	}
	
	void DescriptorHeap::ProcessDeferredFree(u32 frameIndex)
	{
		// TODO: mutex lock
		RYU_ASSERT(frameIndex < FRAME_BUFFER_COUNT);

		auto& indices = m_deferredFreeHandles[frameIndex];
		if (!indices.empty())
		{
			for (auto index : indices)
			{
				--m_size;
				m_freeHandles[m_size] = index;
			}
			indices.clear();
		}
	}
	
	void DescriptorHeap::OnConstruct(DescriptorHeapType type)
	{
		m_type = type;
	}
}

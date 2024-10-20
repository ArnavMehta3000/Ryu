#include "DX12Resources.h"
#include "Graphics/DX12/DX12Core.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics::DX12
{
	DescriptorHeap::DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type)
		: m_heap(nullptr)
		, m_type(type)
	{
	}

	DescriptorHeap::~DescriptorHeap()
	{
		DEBUG_ASSERT(!m_heap, "Descriptor heap not released");
	}

	bool DescriptorHeap::Init(u32 capacity, bool isShaderVisible)
	{
		std::lock_guard lock{ m_mutex };

		DEBUG_ASSERT(capacity && capacity < D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2, "Invalid descriptor heap capacity");
		DEBUG_ASSERT(!(m_type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER && capacity > D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE), "Invalid descriptor heap capacity");

		if (m_type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV ||
			m_type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		{
			isShaderVisible = false;
		}

		Release();  // Release existing heap if any

		ID3D12Device8* const device = Core::GetDevice();
		DEBUG_ASSERT(device);

		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Flags          = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask       = 0;
		desc.NumDescriptors = capacity;
		desc.Type           = m_type;

		HRESULT hr{ S_OK };
		DXCall(hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap)));

		if (FAILED(hr))
		{
			Release();
			return false;
		}

		m_freeHandles = std::move(std::make_unique<u32[]>(capacity));
		m_capacity    = capacity;
		m_size        = 0;

		for (u32 i = 0; i < capacity; i++)
		{
			m_freeHandles[i] = i;
		}

#if defined(RYU_BUILD_DEBUG)
		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			DEBUG_ASSERT(m_deferredFreeIndices[i].empty(), "Invalid deferred free indices");
		}
#endif

		
		m_descriptorSize = device->GetDescriptorHandleIncrementSize(m_type);
		m_cpuStart       = m_heap->GetCPUDescriptorHandleForHeapStart();
		m_gpuStart       = isShaderVisible ? m_heap->GetGPUDescriptorHandleForHeapStart() : CD3DX12_GPU_DESCRIPTOR_HANDLE();

		return true;
	}

	void DescriptorHeap::Release()
	{
		ASSERT(!m_size);
		Core::DeferredRelease(m_heap);
	}

	NODISCARD DescriptorHandle DescriptorHeap::Allocate()
	{
		std::lock_guard lock{ m_mutex };
		DEBUG_ASSERT(m_heap);
		ASSERT(m_size < m_capacity, "Descriptor heap is full");

		const u32 index = m_freeHandles[m_size];
		const u32 offset = index * m_descriptorSize;
		m_size++;

		DescriptorHandle handle;
		handle.CPU.ptr = m_cpuStart.ptr + offset;
		if (IsShaderVisible())
		{
			handle.GPU.ptr = m_gpuStart.ptr + offset;
		}

#if defined(RYU_BUILD_DEBUG)
		handle.Container = this;
		handle.Index     = index;
#endif

		return handle;
	}

	void DescriptorHeap::Free(DescriptorHandle& handle)
	{
		if (!handle.IsValid())
		{
			return;
		}

		std::lock_guard lock{ m_mutex };

		ASSERT(m_heap && m_size                                         , "Descriptor heap is not initialized");
		ASSERT(handle.CPU.ptr >= m_cpuStart.ptr                         , "Invalid descriptor handle");
		
		ASSERT((handle.CPU.ptr - m_cpuStart.ptr) % m_descriptorSize == 0, "Invalid descriptor handle");
#if defined(RYU_BUILD_DEBUG)
		ASSERT(handle.Container == this                                 , "Invalid descriptor handle");
		ASSERT(handle.Index < m_capacity                                , "Invalid descriptor handle");
#endif
		const u32 index = u32((handle.CPU.ptr - m_cpuStart.ptr) / m_descriptorSize);

#if defined(RYU_BUILD_DEBUG)
		ASSERT(handle.Index == index                                    , "Invalid descriptor handle");
#endif

		const u32 frameIndex = Core::GetCurrentFrameIndex();
		m_deferredFreeIndices[frameIndex].push_back(index);
		Core::SetDeferredReleaseFlag();
	}

	void DescriptorHeap::ProcessDeferredFree(u32 frameIndex)
	{
		std::lock_guard lock{ m_mutex };
		ASSERT(frameIndex < FRAME_BUFFER_COUNT, "Invalid frame index");

		std::vector<u32>& indices = m_deferredFreeIndices[frameIndex];

		if (!indices.empty())
		{
			for (u32 index : indices)
			{
				m_size--;
				m_freeHandles[index] = index;
			}

			indices.clear();
		}
	}
}

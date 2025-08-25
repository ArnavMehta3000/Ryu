#include "Graphics/Core/DescriptorAllocator.h"
#include "Graphics/Core/Core.h"
#include "Logging/Logger.h"

namespace Ryu::Gfx
{
	std::mutex DescriptorAllocator::s_allocationMutex;
	std::vector<ComPtr<DX12::DescriptorHeap>> DescriptorAllocator::s_heapPool;

	DescriptorAllocator::DescriptorAllocator(DescriptorHeapType type)
		: m_type(type)
		, m_currentHeap(nullptr)
		, m_currentHandle(D3D12_DEFAULT)
		, m_descriptorSize(0)
		, m_remainingFreeHandles(0)
	{
	}
	
	DescriptorHandle DescriptorAllocator::Allocate(u32 count)
	{
		RYU_LOG_TRACE("Allocating {} descriptor(s)", count);

		if (!m_currentHeap || m_remainingFreeHandles < count)
		{
			m_currentHeap          = RequestNewHeap(m_type);
			m_currentHandle        = m_currentHeap->GetCPUDescriptorHandleForHeapStart();
			m_remainingFreeHandles = DESCRIPTORS_PER_HEAP;
			
			if (m_descriptorSize == 0)
			{
				m_descriptorSize = Core::GetDevice()->GetDescriptorHandleIncrementSize(DX12::ToNative(m_type));
			}
		}

		DescriptorHandle handle(m_currentHandle, {});  // No GPU handle

		m_currentHandle.Offset(count, m_descriptorSize);
		m_remainingFreeHandles -= count;

		return handle;
	}

	void DescriptorAllocator::DestroyAll()
	{
		s_heapPool.clear();
	}
	
	DX12::DescriptorHeap* DescriptorAllocator::RequestNewHeap(DescriptorHeapType type)
	{
		std::lock_guard lock(s_allocationMutex);

		RYU_LOG_DEBUG("Requesting new descriptor heap ({})", EnumToString(type));

		const auto& device = Core::GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC desc
		{
			.Type           = DX12::ToNative(type),
			.NumDescriptors = DESCRIPTORS_PER_HEAP,
			.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask       = 0
		};

		ComPtr<DX12::DescriptorHeap> heap;
		DXCallEx(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap)), device.Get());

		s_heapPool.emplace_back(heap);
		return heap.Get();
	}
}

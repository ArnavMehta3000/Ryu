#include "Graphics/Core/DescriptorHeap.h"
#include "Graphics/Core/Core.h"
#include "Logging/Logger.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
	DescriptorHeap::DescriptorHeap()
		: m_desc({})
		, m_descriptorSize(0)
		, m_numFreeDescriptors(0)
		, m_firstHandle()
		, m_nextFreeHandle()
	{
	}

	DescriptorHeap::~DescriptorHeap()
	{
		RYU_ASSERT(!m_heap, "DescriptorHeap is not properly destructed. Call Destroy() first.");
	}
	
	void DescriptorHeap::Create(DescriptorHeapType type, u32 maxCount)
	{
		m_desc.Type           = DX12::ToNative(type);
		m_desc.NumDescriptors = maxCount;
		m_desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_desc.NodeMask       = 0;

		const auto& device = Core::GetDevice();
		DXCallEx(device->CreateDescriptorHeap(&m_desc, IID_PPV_ARGS(&m_heap)), device.Get());
		
		m_descriptorSize     = device->GetDescriptorHandleIncrementSize(m_desc.Type);
		m_numFreeDescriptors = maxCount;
		m_firstHandle.CPU    = m_heap->GetCPUDescriptorHandleForHeapStart();
		m_firstHandle.GPU    = m_heap->GetGPUDescriptorHandleForHeapStart();

		m_nextFreeHandle     = m_firstHandle;

		RYU_CODE_BLOCK("Name and Log")
		{
			RYU_LOG_DEBUG("Created descriptor heap (max count: {} | type: {})", maxCount, EnumToString(type));
			SetName(fmt::format("Descriptor Heap ({})", EnumToString(type)).c_str());
		}
	}

	void DescriptorHeap::Destroy()
	{
		ComRelease(m_heap);
	}

	DescriptorHandle DescriptorHeap::Allocate(u32 count)
	{
		const bool hasAvailableSpace = HasSpace(count);
		if (!hasAvailableSpace)
		{
			RYU_LOG_ERROR("Not enough space in descriptor heap to allocate {} descriptors", count);
			RYU_ASSERT(hasAvailableSpace, "Descriptor heap out of space! Increase heap size.");
			return {};
		}


		DescriptorHandle handle = m_nextFreeHandle;
		m_nextFreeHandle.Offset(count, m_descriptorSize);
		m_numFreeDescriptors -= count;

		return handle;
	}
	
	bool DescriptorHeap::ValidateHandle(const DescriptorHandle& handle) const
	{
		if (handle.CPU.ptr < m_firstHandle.CPU.ptr
			|| handle.CPU.ptr >= m_firstHandle.CPU.ptr + m_desc.NumDescriptors * m_descriptorSize)
		{
			return false;
		}

		if (handle.GPU.ptr - m_firstHandle.GPU.ptr != handle.CPU.ptr - m_firstHandle.CPU.ptr)
		{
			return false;
		}

		return true;
	}
	
	DescriptorHandle DescriptorHeap::At(u32 arrayIdx) const
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpu{};
		cpu.InitOffsetted(m_firstHandle.CPU, arrayIdx * m_descriptorSize);

		CD3DX12_GPU_DESCRIPTOR_HANDLE gpu{};
		gpu.InitOffsetted(m_firstHandle.GPU, arrayIdx * m_descriptorSize);

		return { cpu, gpu };
	}
}

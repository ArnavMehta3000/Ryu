#include "Graphics/Core/DescriptorHeap.h"
#include "Graphics/Core/Device.h"
#include "Profiling/Profiling.h"
#include <format>

namespace Ryu::Gfx
{
	DescriptorHeap::DescriptorHeap(std::weak_ptr<Device> parent, DescriptorHeapType type, DescriptorHeapFlags flags, u32 numDescriptors)
		: DeviceObject(parent)
		, m_cpuHandle(InvalidCPUHandle)
		, m_gpuHandle(InvalidGPUHandle)
		, m_type(type)
		, m_count(numDescriptors)
		, m_descriptorSize(0)
	{
		OnConstruct(type, flags, numDescriptors);
	}

	DescriptorHeap::~DescriptorHeap()
	{
		OnDestruct();
	}

	void DescriptorHeap::OnConstruct(DescriptorHeapType type, DescriptorHeapFlags flags, u32 numDescriptors)
	{
		RYU_PROFILE_SCOPE();

		m_type = type;
		m_count = numDescriptors;

		const bool isShaderVisible = flags == DescriptorHeapFlags::ShaderVisible;

		D3D12_DESCRIPTOR_HEAP_DESC desc
		{
			.Type           = DX12::ToNative(type),
			.NumDescriptors = numDescriptors,
			.Flags          = DX12::ToNative(flags),
			.NodeMask       = 0
		};

		if (auto parent = GetParent())
		{
			if (DX12::Device* const device = parent->GetDevice())
			{
				m_descriptorSize = device->GetDescriptorHandleIncrementSize(desc.Type);

				DXCallEx(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap)), device);

				DX12::SetObjectName(m_heap.Get(),
					std::format("[{}] Descriptor Heap ({})",
						isShaderVisible ? "Shader Visible" : "Non-Shader Visible",
						EnumToString(type)).c_str());

				m_cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_heap->GetCPUDescriptorHandleForHeapStart());

				if (isShaderVisible)
				{
					m_gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_heap->GetGPUDescriptorHandleForHeapStart());
				}
			}
		}
	}

	void DescriptorHeap::OnDestruct()
	{
		if (m_heap)
		{
			m_heap.Reset();
		}
	}

	bool DescriptorHeap::IsValid() const
	{
		return m_cpuHandle.ptr != 0;
	}

	bool DescriptorHeap::IsShaderVisible() const
	{
		return m_gpuHandle.ptr != 0;
	}
}

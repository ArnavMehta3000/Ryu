#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	class GfxDescriptor
	{
		friend class GfxDescriptorAllocatorBase;
	public:
		GfxDescriptor() = default;

		u32 GetIndex() const noexcept { return m_index; }

		void Increment(u32 increment, u32 multiply = 1)
		{
			m_cpu.ptr += increment * multiply;

			if (m_gpu.ptr != NULL)
			{
				m_gpu.ptr += increment * multiply;
			}

			m_index += multiply;
		}

		bool IsValid() const { return m_cpu.ptr != NULL; }

		operator D3D12_CPU_DESCRIPTOR_HANDLE() const { return m_cpu; }
		operator D3D12_GPU_DESCRIPTOR_HANDLE() const { return m_gpu; }

		bool operator==(const GfxDescriptor& other) const 
		{
			return m_cpu.ptr == other.m_cpu.ptr 
				&& m_index == other.m_index;
		}

	private:
		D3D12_CPU_DESCRIPTOR_HANDLE m_cpu{ NULL };
		D3D12_GPU_DESCRIPTOR_HANDLE m_gpu{ NULL };
		u32 m_index = (u32)-1;
	};
}

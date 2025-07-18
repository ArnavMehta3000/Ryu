#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	class CommandAllocator
	{
	public:
		RYU_GFX_NATIVE(m_allocator)
			
		CommandAllocator() = default;
		explicit CommandAllocator(ComPtr<DX12::CommandAllocator> allocator) : m_allocator(std::move(allocator)) {}

		inline void SetName(const char* name) const { DX12::SetObjectName(m_allocator.Get(), name); }

		inline void Reset() const { DXCall(m_allocator->Reset()); }

	private:
		ComPtr<DX12::CommandAllocator> m_allocator;
	};
}

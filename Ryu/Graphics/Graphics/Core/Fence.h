#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	class Fence
	{
	public:
		RYU_GFX_NATIVE(m_fence)

		Fence() = default;
		Fence(ComPtr<DX12::Fence> fence) : m_fence(std::move(fence)) {}
		
		inline void SetName(const char* name) const { DX12::SetObjectName(m_fence.Get(), name); }

		inline u64 GetCompletedValue() const { return m_fence->GetCompletedValue(); }
		inline void SetEventOnCompletion(u64 value, HANDLE event) { m_fence->SetEventOnCompletion(value, event); }

	private:
		ComPtr<DX12::Fence> m_fence;
	};
}

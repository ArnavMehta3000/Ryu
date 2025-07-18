#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	class CommandQueue
	{
	public:
		RYU_GFX_NATIVE(m_queue)
		CommandQueue() = default;
		CommandQueue(ComPtr<DX12::CommandQueue> queue);

		inline void Signal(Fence& fence, u64 value) const;

	private:
		ComPtr<DX12::CommandQueue> m_queue;
	};
}

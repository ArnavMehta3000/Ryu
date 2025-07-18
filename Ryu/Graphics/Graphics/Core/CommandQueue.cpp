#include "Graphics/Core/CommandQueue.h"
#include "Graphics/Core/Fence.h"

namespace Ryu::Gfx
{
	CommandQueue::CommandQueue(ComPtr<DX12::CommandQueue> queue)
		: m_queue(std::move(queue))
	{
	}
	
	void CommandQueue::Signal(Fence& fence, u64 value) const
	{
		DXCall(m_queue->Signal(fence, value));
	}
}

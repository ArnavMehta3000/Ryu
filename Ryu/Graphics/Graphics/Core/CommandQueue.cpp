#include "Graphics/Core/CommandQueue.h"
#include "Graphics/Core/Device.h"
#include "Graphics/Core/Fence.h"
#include <format>

namespace Ryu::Gfx
{
	CommandQueue::CommandQueue(DeviceWeakPtr parent, CommandListType type, CommandQueuePriority priority)
		: DeviceObject(parent)
	{
		OnConstruct(type, priority);
	}
	
	void CommandQueue::Signal(Fence& fence, u64 value) const
	{
		DXCall(m_queue->Signal(fence, value));
	}

	void CommandQueue::OnConstruct(CommandListType type, CommandQueuePriority priority)
	{
		if (auto parent = GetParent())
		{
			DX12::Device* const device = parent->GetDevice();
			D3D12_COMMAND_QUEUE_DESC desc
			{
				.Type     = DX12::ToNative(type),
				.Priority = DX12::ToNative(priority),
				.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
				.NodeMask = 0
			};

			DXCallEx(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_queue)), device);
			SetName(fmt::format("Command Queue ({})", EnumToString(type)).c_str());
		}
	}

	void CommandQueue::OnDestruct()
	{
		if (m_queue)
		{
			m_queue.Reset();
		}
	}
}

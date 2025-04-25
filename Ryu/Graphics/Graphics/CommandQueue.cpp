#include "Graphics/CommandQueue.h"
#include "Graphics/Device.h"

namespace Ryu::Gfx
{
	CommandQueue::CommandQueue(Device* parent, D3D12_COMMAND_LIST_TYPE type)
		: DeviceObject(parent)
		, m_type(type)
	{
		D3D12_COMMAND_QUEUE_DESC desc
		{
			.Type     = type,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0
		};

		m_fence = std::make_shared<Fence>(parent, "CommandQueue Fence");

		DXCallEx(parent->GetDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_cmdQueue)), parent->GetDevice().Get());
		DX12::SetObjectName(m_cmdQueue.Get(), std::format("{} CommandQueue", Internal::CommandListTypeToString(type)).c_str());

		DXCall(m_cmdQueue->GetTimestampFrequency(&m_timestampFrequency));
	}
}

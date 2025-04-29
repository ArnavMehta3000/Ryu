#include "Graphics/CmdQueue.h"
#include "Graphics/Device.h"

namespace Ryu::Gfx
{
	CmdQueue::CmdQueue(Device* parent, D3D12_COMMAND_LIST_TYPE type)
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

		m_fence = Memory::CreateRef<Fence>(parent, "CmdQueue Fence");

		DXCallEx(parent->GetDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_cmdQueue)), parent->GetDevice());
		DX12::SetObjectName(m_cmdQueue.Get(), std::format("{} CmdQueue", Internal::CommandListTypeToString(type)).c_str());

		DXCall(m_cmdQueue->GetTimestampFrequency(&m_timestampFrequency));
	}
}

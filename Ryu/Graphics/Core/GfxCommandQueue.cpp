#include "Graphics/Core/GfxCommandQueue.h"
#include "Graphics/Core/GfxDevice.h"
#include "Graphics/Core/GfxFence.h"
#include "Graphics/Core/GfxCommandList.h"
#include <vector>

namespace Ryu::Gfx
{
	CommandQueue::CommandQueue(Device* parent, D3D12_COMMAND_LIST_TYPE type, std::string_view name)
		: DeviceChild(parent)
		, m_timestampFrequency(0)
		, m_type(type)
	{
		D3D12_COMMAND_QUEUE_DESC desc
		{
			.Type     = type,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0
		};

		DX12::Device* device = GetDevice()->GetNativeDevice();

		DXCall(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_cmdQueue)));
		DX12::SetObjectName(m_cmdQueue.Get(), name.data());

		if (type != D3D12_COMMAND_LIST_TYPE_COPY)
		{
			DXCall(m_cmdQueue->GetTimestampFrequency(&m_timestampFrequency));
		}
	}
	
	void CommandQueue::Signal(const Fence& fence, u64 value)
	{
		m_cmdQueue->Signal(fence, value);
	}
	
	void CommandQueue::ExecuteCommandList(const CommandList& cmdList)
	{
		ID3D12CommandList* cmdLists[] = { cmdList.GetNative() };
		m_cmdQueue->ExecuteCommandLists(1, cmdLists);
	}
	
	void CommandQueue::ExecuteCommandLists(std::span<const CommandList> cmdLists)
	{
		std::vector<ID3D12CommandList*> nativeCmdLists(cmdLists.size(), nullptr);
		for (u32 i = 0; i < cmdLists.size(); i++)
		{
			nativeCmdLists[i] = cmdLists[i].GetNative();
		}
		m_cmdQueue->ExecuteCommandLists(u32(cmdLists.size()), nativeCmdLists.data());
	}
}

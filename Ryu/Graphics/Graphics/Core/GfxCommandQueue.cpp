#include "Graphics/Core/GfxCommandQueue.h"
#include "Graphics/Core/GfxCommandList.h"
#include "Graphics/Core/GfxDevice.h"
#include "Graphics/Core/GfxFence.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
	void GfxCommandQueue::Create(GfxDevice* device, CommandListType type, std::string_view name)
	{
		m_type = type;

		DX12::Device* dx12Device = device->GetDevice();

		D3D12_COMMAND_QUEUE_DESC desc
		{
			.Type     = DX12::ToNative(type),
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0
		};

		DXCallEx(dx12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_cmdQueue)), dx12Device);
		SetName(name.data());

		if (m_type != type)
		{
			m_cmdQueue->GetTimestampFrequency(&m_timestampFrequency);
		}
	}
	
	void GfxCommandQueue::Signal(GfxFence& fence, u64 fenceValue)
	{
		RYU_ASSERT(m_cmdQueue, "Command queue must be created before signaling.");
		DXCall(m_cmdQueue->Signal(fence, fenceValue));
	}
	
	void GfxCommandQueue::Wait(GfxFence& fence, u64 fenceValue)
	{
		RYU_ASSERT(m_cmdQueue, "Command queue must be created before waiting.");
		DXCall(m_cmdQueue->Wait(fence, fenceValue));
	}
	
	void GfxCommandQueue::ExecuteCommandLists(std::span<GfxCommandList*> cmdLists)
	{
		if (cmdLists.empty())
		{
			return;
		}

		for (GfxCommandList* cmdList : cmdLists)
		{
			cmdList->WaitAll();
		}

		// Convert to native
		std::vector<ID3D12CommandList*> nativeCmdLists(cmdLists.size(), nullptr);
		for (u32 i = 0; i < cmdLists.size(); ++i)
		{
			nativeCmdLists[i] = cmdLists[i]->GetNative().Get();
		}

		m_cmdQueue->ExecuteCommandLists((u32)nativeCmdLists.size(), nativeCmdLists.data());

		for (GfxCommandList* cmdList : cmdLists)
		{
			cmdList->SignalAll();
		}
	}
}

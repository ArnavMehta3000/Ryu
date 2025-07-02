#include "Graphics/Core/CommandContext.h"
#include "Graphics/Core/CommandList.h"
#include "Graphics/Core/Device.h"
#include "Logger/Logger.h"
#include "Profiling/Profiling.h"
#include <array>
#include <vector>
#include <ranges>

namespace Ryu::Gfx
{
	RYU_LOG_DECLARE_CATEGORY(GFXCommandCtx);

	CommandCtx::CommandCtx(std::weak_ptr<Device> parent, CommandListType type, CommandQueuePriority priority)
		: DeviceObject(parent), m_type(type)
	{
		OnConstruct(type, priority);
	}

	void CommandCtx::OnConstruct(CommandListType type, CommandQueuePriority priority)
	{
		m_type = type;
		if (auto parent = GetParent())
		{
			D3D12_COMMAND_QUEUE_DESC desc
			{
				.Type     = DX12::ToNative(m_type),
				.Priority = DX12::ToNative(priority),
				.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
				.NodeMask = 0
			};

			DX12::Device* const device = parent->GetDevice();
			DXCallEx(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_cmdQueue)), device);

			DX12::SetObjectName(m_cmdQueue.Get(), std::format("Command Queue [{}]", EnumToString(m_type)).data());
		}
		else
		{
			RYU_LOG_ERROR(LogGFXCommandCtx, "CommandCtx::CreateCommandQueue failed, parent device is not valid!");
		}
	}

	void CommandCtx::ExecuteCommandList(CommandList* cmdList)
	{
		RYU_PROFILE_SCOPE();

		if (m_cmdQueue && cmdList && cmdList->GetCommandList())
		{
			std::array<ID3D12CommandList*, 1> cmdLists = { cmdList->GetCommandList() };
			m_cmdQueue->ExecuteCommandLists(static_cast<u32>(cmdLists.size()), cmdLists.data());
		}
	}

	void CommandCtx::ExecuteCommandLists(std::span<CommandList*> cmdLists)
	{
		RYU_PROFILE_SCOPE();
		if (cmdLists.empty())
		{
			return;
		}

		std::vector<ID3D12CommandList*> nativeCmdLists(cmdLists.size(), nullptr);
		std::ranges::transform(cmdLists, nativeCmdLists.begin(), [](CommandList* cmdList) { return cmdList->GetCommandList(); });

		if (nativeCmdLists.size() > 0 && m_cmdQueue)
		{
			m_cmdQueue->ExecuteCommandLists(static_cast<u32>(nativeCmdLists.size()), nativeCmdLists.data());
		}
	}

	void CommandCtx::Signal(Fence& fence, u64 value)
	{
		fence.SignalGPU(this, value);
	}

	void CommandCtx::Wait(Fence& fence, u64 value)
	{
		fence.WaitGPU(this, value);
	}

	void CommandCtx::Flush(Fence& fence)
	{
		const u64 fenceValue = fence.GetNextValue();
		Signal(fence, fenceValue);
		fence.WaitCPU(fenceValue);
	}
}

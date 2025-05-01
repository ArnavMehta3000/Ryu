#include "Graphics/CommandContext.h"
#include "Graphics/Device.h"
#include "Profiling/Profiling.h"

namespace Ryu::Gfx
{
	CommandContext::CommandContext(Device* parent, CmdListType type)
		: DeviceObject(parent)
		, m_frameIndex(0)
	{
		RYU_PROFILE_SCOPE();
		DX12::Device* const device = parent->GetDevice();
		D3D12_COMMAND_LIST_TYPE cmdListType = DX12::GetCmdListType(type);
		const std::string_view cmdListTypeName = Internal::CommandListTypeToString(cmdListType);

		// Create the command queue
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Type     = cmdListType;
		queueDesc.Flags    = static_cast<D3D12_COMMAND_QUEUE_FLAGS>(CmdListFlags::None);
		queueDesc.Priority = static_cast<i32>(CmdQueuePriority::Normal);
		queueDesc.NodeMask = 0;
		DXCallEx(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_cmdQueue)), device);

		DX12::SetObjectName(m_cmdQueue.Get(), std::format("{} Command Queue", cmdListTypeName).c_str());

		// Create the command allocator
		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			CommandFrame& frame = m_cmdFrames[i];
			DXCallEx(device->CreateCommandAllocator(cmdListType, IID_PPV_ARGS(&frame.CmdAllocator)), device);
			DX12::SetObjectName(frame.CmdAllocator.Get(), std::format("{} Command Allocator - {}", cmdListTypeName, i).c_str());
		}

		// Create the command list and start it closed
		DXCallEx(device->CreateCommandList(0, cmdListType, m_cmdFrames[0].CmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_cmdList)), device);
		m_cmdList->Close();

		DX12::SetObjectName(m_cmdList.Get(), std::format("{} Command List", cmdListTypeName).c_str());
	}
	
	CommandContext::~CommandContext()
	{
		
	}
	
	void CommandContext::BeginFrame()
	{
		RYU_PROFILE_SCOPE();
		CommandFrame& frame = m_cmdFrames[m_frameIndex];
		frame.Wait();

		// Resetting the allocator will free the memory used by previously recorded commands
		DXCall(frame.CmdAllocator->Reset());

		// Resetting the command list will reopen it for recording new commands
		DXCall(m_cmdList->Reset(frame.CmdAllocator.Get(), nullptr));
	}
	
	void CommandContext::EndFrame()
	{
		RYU_PROFILE_SCOPE();

		DXCall(m_cmdList->Close());

		// Execute the command list on the GPU
		ID3D12CommandList* const cmdLists[] = { m_cmdList.Get() };
		m_cmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
		
		m_frameIndex = (m_frameIndex + 1) % FRAME_BUFFER_COUNT;  // Wrap frame index
	}
}

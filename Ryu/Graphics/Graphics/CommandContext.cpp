#include "Graphics/CommandContext.h"
#include "Graphics/Device.h"
#include "Profiling/Profiling.h"
#include <libassert/assert.hpp>

namespace Ryu::Gfx
{
	CommandContext::CommandContext(Device* parent, CmdListType type)
		: DeviceObject(parent)
		, m_frameIndex(0)
		, m_fenceEvent(nullptr)
		, m_fenceValue(0)
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

		// Create the fence
		DXCallEx(device->CreateFence(0, static_cast<D3D12_FENCE_FLAGS>(FenceFlag::None), IID_PPV_ARGS(&m_fence)), device);
		DX12::SetObjectName(m_fence.Get(), std::format("Command Context ({}) Fence", cmdListTypeName).c_str());

		m_fenceEvent = ::CreateEventEx(nullptr, L"CommandContextFenceEvent", 0, EVENT_ALL_ACCESS);
		DEBUG_ASSERT(m_fenceEvent);
	}
	
	CommandContext::~CommandContext()
	{
		RYU_PROFILE_SCOPE();
		Flush();
		m_fence.Reset();
		m_fenceValue = 0;

		::CloseHandle(m_fenceEvent);
		m_fenceEvent = nullptr;

		m_cmdQueue.Reset();
		m_cmdList.Reset();

		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			m_cmdFrames[i].ReleaseAllocator();
		}
	}
	
	void CommandContext::BeginFrame()
	{
		RYU_PROFILE_SCOPE();
		CommandFrame& frame = m_cmdFrames[m_frameIndex];
		frame.Wait(m_fenceEvent, m_fence.Get());

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

		// If frames fence value is less than the signalled fence value means that the GPU is 
		// done executing commands for this frame and we can use it to record new commands
		u64& fenceValue = m_fenceValue;
		++fenceValue;
		CommandFrame& frame = m_cmdFrames[m_frameIndex];
		frame.FenceValue = fenceValue;
		
		DXCall(m_cmdQueue->Signal(m_fence.Get(), m_fenceValue));
		
		m_frameIndex = (m_frameIndex + 1) % FRAME_BUFFER_COUNT;  // Wrap frame index
	}

	void CommandContext::Flush()
	{
		RYU_PROFILE_SCOPE();
		DX12::Fence* const fence = m_fence.Get();
		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			m_cmdFrames[i].Wait(m_fenceEvent, fence);
		}
		m_frameIndex = 0;
	}
	
	void CommandContext::CommandFrame::Wait(HANDLE fenceEvent, DX12::Fence* fence)
	{
		DEBUG_ASSERT(fence && fenceEvent);
		RYU_PROFILE_SCOPE();

		// If completed value is less than current 'FenceValue' then the GPU is still executing commands
		// Meaning it has still not yet reached the 'm_cmdQueue->Signal' command
		if (fence->GetCompletedValue() < FenceValue)
		{
			DXCall(fence->SetEventOnCompletion(FenceValue, fenceEvent));

			// Wait until the fence has triggered the event that its current value has reached FenceValue
			// Indicating the the command queue has finished executing commands
			::WaitForSingleObject(fenceEvent, INFINITE);
		}
	}
}

#include "Graphics/Core/CommandContext.h"
#include "Graphics/Core/Device.h"
#include "Logger/Assert.h"
#include "Logger/Logger.h"
#include <format>

namespace Ryu::Gfx
{
	RYU_LOG_DECLARE_CATEGORY(CommandCtx);

	
	CommandContext::CommandContext(DeviceWeakPtr parent, CommandListType type)
		: DeviceObject(parent)
	{
		OnConstruct(type);
	}
	
	CommandContext::~CommandContext()
	{
		// If there is a valid fence, that means we have not properly destructed
		// -> This fixes the crash when `OnDestruct` is called from Destroy and the destructor
		// -> Causing double calls to `Flush` -> `Wait` (where the assert fails on the fence)
		if (m_fence)
		{
			OnDestruct();
		}
		// Ensure everything is released
		RYU_ASSERT(!m_cmdQueue && !m_fence && !m_cmdList);
	}

	void CommandContext::BeginFrame(PipelineState* pso)
	{
		CommandFrame& frame = m_cmdFrames[m_frameIndex];

		frame.Wait(m_fenceEvent, m_fence);
		frame.Allocator.Reset();
		m_cmdList.Reset(frame.Allocator, pso);
	}

	void CommandContext::EndFrame()
	{
		m_cmdList.Close();

		ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
		m_cmdQueue.Get()->ExecuteCommandLists(_countof(cmdLists), cmdLists);

		u64& fenceValue = m_fenceValue;
		++fenceValue;

		CommandFrame& frame = m_cmdFrames[m_frameIndex];
		frame.FenceValue = fenceValue;
		
		m_cmdQueue.Signal(m_fence, fenceValue);

		m_frameIndex = (m_frameIndex + 1) % FRAME_BUFFER_COUNT;
	}

	void CommandContext::SetResourceBarrier(const CD3DX12_RESOURCE_BARRIER& barrier)
	{
		RYU_ASSERT(m_cmdList, "Command list is not initialized.");
		m_cmdList.Get()->ResourceBarrier(1, &barrier);
	}

	void CommandContext::SetResourceBarriers(std::span<const CD3DX12_RESOURCE_BARRIER> barriers)
	{
		RYU_ASSERT(m_cmdList, "Command list is not initialized.");
		m_cmdList.Get()->ResourceBarrier(static_cast<u32>(barriers.size()), barriers.data());
	}

	void CommandContext::Flush()
	{
		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			m_cmdFrames[i].Wait(m_fenceEvent, m_fence);
		}
		m_frameIndex = 0;
	}

	void CommandContext::OnConstruct(CommandListType type)
	{
		if (DevicePtr parent = GetParent())
		{
			// Init queue
			m_cmdQueue.Initialize(parent, type, CommandQueuePriority::Normal);

			// Init allocators
			for (u32 i = 0; i < m_cmdFrames.size(); i++)
			{
				m_cmdFrames[i].Allocator.Initialize(parent->weak_from_this(), type);
				m_cmdFrames[i].Allocator.SetName(std::format("{} Command Allocator ({})", EnumToString(type), i).c_str());
			}

			// Init cmd list
			m_cmdList.Initialize(parent->weak_from_this(), m_cmdFrames[0].Allocator, nullptr, type);

			// Init fence
			m_fence.Initialize(parent, 0, FenceFlag::None);
			m_fenceEvent = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		}
	}
	
	void CommandContext::OnDestruct()
	{
		Flush();
		m_fence.Destroy();

		if (m_fenceEvent)
		{
			::CloseHandle(m_fenceEvent);
			m_fenceEvent = nullptr;
		}

		m_cmdQueue.Destroy();
		m_cmdList.Destroy();

		for (u32 i = 0; i < m_cmdFrames.size(); i++)
		{
			m_cmdFrames[i].Release();
		}
	}
	
	void CommandContext::CommandFrame::Wait(HANDLE fenceEvent, Fence& fence)
	{
		RYU_ASSERT(fence.Get() && fenceEvent, "Fence is not initialized.");
		
		// If the current (completed) 'fenceValue' is less than 'FenceValue'
		// We know that the GPU has not reached the fence value yet (not finished executing the command lists)

		const u64 completedValue = fence.Get()->GetCompletedValue();
		if (completedValue < FenceValue)
		{
			if (::WaitForSingleObjectEx(fenceEvent, TIMEOUT_DURATION, FALSE) == WAIT_TIMEOUT)
			{
				RYU_LOG_ERROR(LogCommandCtx, "Move to next frame timed out!");
			}
		}
	}

	void CommandContext::CommandFrame::Release()
	{
		Allocator.Destroy();
		FenceValue = 0;
	}
}

#include "Graphics/Core/CommandContext.h"
#include "Graphics/Core/Device.h"
#include "Logger/Assert.h"
#include "Logger/Logger.h"
#include <format>

namespace Ryu::Gfx
{
	RYU_LOG_DECLARE_CATEGORY(Command);

	CommandContext::CommandContext(CommandList* cmdList, CommandAllocator* allocator, PipelineState* pso)
		: m_cmdList(cmdList)
		, m_allocator(allocator)
	{
		Begin(pso);
	}

	CommandContext::~CommandContext()
	{
		End();
	}

	void CommandContext::Begin(PipelineState* pso)
	{
		m_allocator->Reset();
		m_cmdList->Reset(*m_allocator, pso);
	}

	void CommandContext::End()
	{
		m_cmdList->Close();
	}
	
	void CommandContext::SetResourceBarrier(const CD3DX12_RESOURCE_BARRIER& barrier)
	{
		m_cmdList->Get()->ResourceBarrier(1, &barrier);
	}
	
	void CommandContext::SetResourceBarriers(std::span<const CD3DX12_RESOURCE_BARRIER> barriers)
	{
		m_cmdList->Get()->ResourceBarrier(static_cast<u32>(barriers.size()), barriers.data());
	}
	
	
	
	
	
	
	
	
	
	Command::Command(DeviceWeakPtr parent, CommandListType type)
		: DeviceObject(parent)
	{
		OnConstruct(type);
	}
	
	Command::~Command()
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

	void Command::BeginFrame(PipelineState* pso)
	{
		CommandFrame& frame = m_cmdFrames[m_frameIndex];

		frame.Wait(m_fenceEvent, m_fence);
		frame.Allocator.Reset();
		m_cmdList.Reset(frame.Allocator, pso);
	}

	void Command::EndFrame()
	{
		m_cmdList.Close();

		ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
		m_cmdQueue.Get()->ExecuteCommandLists(_countof(cmdLists), cmdLists);

		CommandFrame& frame = m_cmdFrames[m_frameIndex];

		u64& fenceValue = m_fenceValue;
		++fenceValue;
		frame.FenceValue = fenceValue;
		
		m_cmdQueue.Signal(m_fence, m_fenceValue);

		m_frameIndex = (m_frameIndex + 1) % FRAME_BUFFER_COUNT;
	}

	void Command::SetResourceBarrier(const CD3DX12_RESOURCE_BARRIER& barrier)
	{
		RYU_ASSERT(m_cmdList, "Command list is not initialized.");
		m_cmdList.Get()->ResourceBarrier(1, &barrier);
	}

	void Command::SetResourceBarriers(std::span<const CD3DX12_RESOURCE_BARRIER> barriers)
	{
		RYU_ASSERT(m_cmdList, "Command list is not initialized.");
		m_cmdList.Get()->ResourceBarrier(static_cast<u32>(barriers.size()), barriers.data());
	}

	void Command::Flush()
	{
		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			m_cmdFrames[i].Wait(m_fenceEvent, m_fence);
		}
		m_frameIndex = 0;
	}

	void Command::OnConstruct(CommandListType type)
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
	
	void Command::OnDestruct()
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
	
	void Command::CommandFrame::Wait(HANDLE fenceEvent, Fence& fence)
	{
		RYU_ASSERT(fence.Get() && fenceEvent, "Fence is not initialized.");

		if (fence.Get()->GetCompletedValue() < FenceValue)
		{
			DXCall(fence.Get()->SetEventOnCompletion(FenceValue, fenceEvent));
			if (::WaitForSingleObjectEx(fenceEvent, TIMEOUT_DURATION, FALSE) == WAIT_TIMEOUT)
			{
				RYU_LOG_ERROR(LogCommand, "Move to next frame timed out!");
			}
		}
	}

	void Command::CommandFrame::Release()
	{
		Allocator.Destroy();
		FenceValue = 0;
	}
}

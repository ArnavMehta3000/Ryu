#include "Graphics/CommandContext.h"
#include "Graphics/Core/GfxDevice.h"
#include "Common/Assert.h"
#include "Globals/Globals.h"

namespace Ryu::Gfx
{
	CommandContext::CommandContext(GfxDevice* device, D3D12_COMMAND_LIST_TYPE type)
		: m_device(device)
		, m_type(type)
	{
		RYU_ASSERT(m_device, "GfxDevice cannot be null");

		CreateCommandQueue();
		CreateCommandAllocators();
		CreateCommandList();
		CreateSynchronizationObjects();

		RYU_LOG_TRACE("CommandContext created");
	}

	CommandContext::~CommandContext()
	{
		WaitForGPU();

		if (m_fenceEvent)
		{
			::CloseHandle(m_fenceEvent);
		}

		ComRelease(m_cmdQueue);
		ComRelease(m_cmdList);
		ComRelease(m_fence);

		for (auto& allocator : m_cmdAllocators)
		{
			ComRelease(allocator);
		}
	}

	void CommandContext::BeginFrame(u32 frameIndex, DX12::PipelineState* pipelineState)
	{
		m_frameIndex = frameIndex;

		// Reset allocator and command list
		DXCall(m_cmdAllocators[m_frameIndex]->Reset());
		DXCall(m_cmdList->Reset(m_cmdAllocators[m_frameIndex].Get(), pipelineState));
	}

	void CommandContext::EndFrame()
	{
		DXCall(m_cmdList->Close());
	}

	void CommandContext::ExecuteCommandList()
	{
		ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
		m_cmdQueue->ExecuteCommandLists(1, cmdLists);
	}

	void CommandContext::WaitForGPU()
	{
		// Schedule a Signal command in the queue
		DXCall(m_cmdQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));

		// Wait until the fence is signaled
		DXCall(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
		::WaitForSingleObject(m_fenceEvent, INFINITE);

		// Increment fence value for current frame
		m_fenceValues[m_frameIndex]++;
	}

	void CommandContext::MoveToNextFrame(u32 nextFrameIndex)
	{
		const u64 currentFenceValue = m_fenceValues[m_frameIndex];
		DXCall(m_cmdQueue->Signal(m_fence.Get(), currentFenceValue));

		m_frameIndex = nextFrameIndex;

		// Wait if the next frame is not ready
		if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
		{
			DXCall(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
			::WaitForSingleObject(m_fenceEvent, INFINITE);
		}

		// Set fence value for next frame
		m_fenceValues[m_frameIndex] = currentFenceValue + 1;
	}

	u64 CommandContext::SignalFence()
	{
		const u64 fenceValue = m_fenceValues[m_frameIndex];
		DXCall(m_cmdQueue->Signal(m_fence.Get(), fenceValue));
		m_fenceValues[m_frameIndex]++;
		return fenceValue;
	}

	void CommandContext::WaitForFenceValue(u64 fenceValue)
	{
		if (m_fence->GetCompletedValue() < fenceValue)
		{
			DXCall(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent));
			::WaitForSingleObject(m_fenceEvent, INFINITE);
		}
	}

	void CommandContext::ExecuteImmediate(std::function<void(ID3D12GraphicsCommandList*)> recordFunc)
	{
		BeginFrame(m_frameIndex, nullptr);
		recordFunc(m_cmdList.Get());
		EndFrame();
		ExecuteCommandList();
		WaitForGPU();
	}

	void CommandContext::CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC desc
		{
			.Type     = m_type,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0
		};

		DXCall(m_device->GetDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_cmdQueue)));

		const char* name = (m_type == D3D12_COMMAND_LIST_TYPE_DIRECT) ? "Graphics Command Queue" : "Command Queue";
		DX12::SetObjectName(m_cmdQueue.Get(), name);
	}

	void CommandContext::CreateCommandAllocators()
	{
		static constexpr std::array allocatorNames =
		{
			"Command Allocator 0",
			"Command Allocator 1",
			"Command Allocator 2",
			"Command Allocator 3"
		};
		static_assert(FRAME_BUFFER_COUNT <= allocatorNames.size());

		for (u32 i = 0; i < m_cmdAllocators.size(); i++)
		{
			DXCall(m_device->GetDevice()->CreateCommandAllocator(
				m_type,
				IID_PPV_ARGS(&m_cmdAllocators[i])));

			DX12::SetObjectName(m_cmdAllocators[i].Get(), allocatorNames[i]);
		}
	}

	void CommandContext::CreateCommandList()
	{
		DXCall(m_device->GetDevice()->CreateCommandList(
			0,
			m_type,
			m_cmdAllocators[0].Get(),
			nullptr,
			IID_PPV_ARGS(&m_cmdList)));

		const char* name = (m_type == D3D12_COMMAND_LIST_TYPE_DIRECT) ? "Graphics Command List" : "Command List";
		DX12::SetObjectName(m_cmdList.Get(), name);

		m_cmdList->Close();
	}

	void CommandContext::CreateSynchronizationObjects()
	{
		DXCall(m_device->GetDevice()->CreateFence(
			0,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&m_fence)));

		DX12::SetObjectName(m_fence.Get(), "Frame Fence");

		m_fenceEvent = ::CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		RYU_ASSERT(m_fenceEvent, "Failed to create fence event");

		// Initialize fence values
		for (u32 i = 0; i < m_fenceValues.size(); i++)
		{
			m_fenceValues[i] = 1;
		}
	}
}

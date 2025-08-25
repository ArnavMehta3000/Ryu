#include "Graphics/Core/CommandContext.h"
#include "Graphics/Core/Core.h"
#include "Profiling/Profiling.h"
#include "Common/Assert.h"
#include "Logging/Logger.h"

namespace Ryu::Gfx
{
	void CommandContext::CommandFrame::Wait(HANDLE fenceEvent, const ComPtr<DX12::Fence>& fence)
	{
		// If the current (completed) 'fenceValue' is less than 'FenceValue'
		// We know that the GPU has not reached the fence value yet
		// -> not finished executing the command lists / reached the 'm_cmdQieie->Signal' command
		if (fence.Get()->GetCompletedValue() < FenceValue)
		{
			// Signal the fence event when the fence's current value reaches 'FenceValue'
			DXCall(fence.Get()->SetEventOnCompletion(FenceValue, fenceEvent));

			// Wait for the fence event
			if (::WaitForSingleObjectEx(fenceEvent, TIMEOUT_DURATION, FALSE) == WAIT_TIMEOUT)
			{
				RYU_LOG_ERROR("Move to next frame timed out!");
			}
		}
	}

	void CommandContext::CommandFrame::Release()
	{
		ComRelease(Allocator);
	}

	CommandContext::~CommandContext()
	{
		RYU_ASSERT(!m_cmdQueue && !m_fence && !m_cmdList,
			"CommandContext is not properly destructed. Call Destroy() first.");
	}

	void CommandContext::Create(CommandListType type)
	{
		RYU_PROFILE_SCOPE();

		auto* device = Core::GetDevice().Get();

		CreateCommandQueue(device, type);
		CreateCommandAllocators(device, type);
		CreateCommandList(device, type);
		CreateSynchronization(device);
	}

	void CommandContext::Destroy()
	{
		RYU_PROFILE_SCOPE();

		// Assume fkush has been called externally

		ComRelease(m_fence);

		if (m_fenceEvent)
		{
			::CloseHandle(m_fenceEvent);
			m_fenceEvent = nullptr;
		}

		ComRelease(m_cmdQueue);
		ComRelease(m_cmdList);

		for (u32 i = 0; i < m_cmdFrames.size(); i++)
		{
			m_cmdFrames[i].Release();
		}
	}

	void CommandContext::BeginFrame()  // Wait for the current frame to be singalled
	{
		RYU_PROFILE_SCOPE();

		CommandFrame& frame = m_cmdFrames[m_frameIndex];
		frame.Wait(m_fenceEvent, m_fence);

		// Reset the command buffers for this frame
		frame.Allocator->Reset();
		m_cmdList->Reset(frame.Allocator.Get(), nullptr);
	}

	void CommandContext::EndFrame()  // Signal the fence with a new value
	{
		RYU_PROFILE_SCOPE();

		m_cmdList->Close();

		std::array<ID3D12CommandList*, 1> cmdLists = { m_cmdList.Get() };
		m_cmdQueue.Get()->ExecuteCommandLists((u32)cmdLists.size(), cmdLists.data());

		++m_fenceValue;

		CommandFrame& frame = m_cmdFrames[m_frameIndex];
		frame.FenceValue = m_fenceValue;

		m_cmdQueue->Signal(m_fence.Get(), m_fenceValue);

		m_frameIndex = (m_frameIndex + 1) % FRAME_BUFFER_COUNT;
	}

	void CommandContext::Flush()
	{
		RYU_LOG_DEBUG("Flushing all command frames");

		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			m_cmdFrames[i].Wait(m_fenceEvent, m_fence);
		}
		m_frameIndex = 0;
	}

	void CommandContext::CreateCommandQueue(DX12::Device* device, CommandListType type)
	{

		D3D12_COMMAND_QUEUE_DESC desc
		{
			.Type = DX12::ToNative(type),
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0
		};

		DXCallEx(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_cmdQueue)), device);
		DX12::SetObjectName(m_cmdQueue.Get(), fmt::format("Command Queue ({})", EnumToString(type)).c_str());
	}

	void CommandContext::CreateCommandAllocators(DX12::Device* device, CommandListType type)
	{
		D3D12_COMMAND_LIST_TYPE nativeType = DX12::ToNative(type);
		for (u32 i = 0; i < m_cmdFrames.size(); i++)
		{
			DXCallEx(device->CreateCommandAllocator(nativeType, IID_PPV_ARGS(&m_cmdFrames[i].Allocator)), device);

			DX12::SetObjectName(m_cmdFrames[i].Allocator.Get(),
				fmt::format("{} Command Allocator ({})", EnumToString(type), i).c_str());
		}

	}

	void CommandContext::CreateCommandList(DX12::Device* device, CommandListType type)
	{
		// Create the graphics command list using the first allocator
		DXCallEx(device->CreateCommandList(
			0,                                    // Node mask
			DX12::ToNative(type),
			m_cmdFrames[0].Allocator.Get(),
			nullptr,                              // PSO
			IID_PPV_ARGS(&m_cmdList)),
		device);

		m_cmdList->Close();
		DX12::SetObjectName(m_cmdList.Get(), fmt::format("Command List ({})", EnumToString(type)).c_str());
	}

	void CommandContext::CreateSynchronization(DX12::Device* device)
	{
		DXCallEx(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)), device);
		DX12::SetObjectName(m_fence.Get(), "Frame Fence");

		m_fenceEvent = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		RYU_ASSERT(m_fenceEvent, "Failed to create fence event.");
	}
}

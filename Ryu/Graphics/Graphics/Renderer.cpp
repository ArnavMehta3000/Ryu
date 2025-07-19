#include "Graphics/Renderer.h"
#include "Graphics/Core/CommandContext.h"
#include "Profiling/Profiling.h"
#include "Logger/Logger.h"
#include "Logger/Assert.h"
#include <ranges>

namespace Ryu::Gfx
{
	RYU_LOG_DECLARE_CATEGORY(Renderer);

	Renderer::Renderer(HWND window)
	{
		RYU_PROFILE_SCOPE();

		m_device = Device::Create();
		DeviceWeakPtr parent = m_device;

		for (u32 i = 0; i < m_cmdAllocators.size(); i++)
		{
			m_cmdAllocators[i].Initialize(parent, CommandListType::Direct);
		}
		
		m_fenceEvent = ::CreateEvent(nullptr, false, false, nullptr);
		m_fence.Initialize(parent, 0, FenceFlag::None);
		m_cmdQueue.Initialize(parent, CommandListType::Direct, CommandQueuePriority::Normal);
		m_cmdList.Initialize(parent, m_cmdAllocators[0], CommandListType::Direct);
		m_rtvHeap.Initialize(parent, DescriptorHeapType::RTV, DescriptorHeapFlags::None, FRAME_BUFFER_COUNT);
		
		m_swapChain.Initialize(m_device, m_cmdQueue, m_rtvHeap, window, BACK_BUFFER_FORMAT);

		WaitForGPU();
	}
	
	Renderer::~Renderer()
	{
		RYU_PROFILE_SCOPE();

		if (m_device)
		{
			RYU_LOG_DEBUG(LogRenderer, "Waiting for GPU");
			WaitForGPU();

			RYU_LOG_DEBUG(LogRenderer, "Destroying swapchain");
			m_swapChain.Destroy();

			m_fence.Destroy();
			m_rtvHeap.Destroy();
			m_cmdList.Destroy();
			m_cmdQueue.Destroy();

			for (auto& allocator : m_cmdAllocators)
			{
				allocator.Destroy();
			}

			if (m_fenceEvent)
			{
				::CloseHandle(m_fenceEvent);
			}

			RYU_LOG_DEBUG(LogRenderer, "Destroying graphics device");
			Device::Destroy(*m_device);
			RYU_ASSERT(m_device.use_count() == 1);
			m_device.reset();
		}

	}

	void Renderer::Render()
	{
		RYU_PROFILE_SCOPE();

		PopulateCommandList();

		ID3D12CommandList* ppCommandLists[] = { m_cmdList.Get() };
		m_cmdQueue.Get()->ExecuteCommandLists(1, ppCommandLists);

		m_swapChain.Present();

		MoveToNextFrame();
	}

	void Renderer::OnResize(u32 width, u32 height)
	{
		RYU_PROFILE_SCOPE();

		WaitForGPU();
		m_swapChain.Resize(m_rtvHeap, width, height);
	}

	void Renderer::PopulateCommandList()
	{
		RYU_PROFILE_SCOPE();
		
		const u32 currentBackBufferIndex = m_swapChain.GetFrameIndex();

		// Reset command allocator and list
		m_cmdAllocators[currentBackBufferIndex].Reset();
		m_cmdList.Reset(m_cmdAllocators[currentBackBufferIndex], nullptr);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
			m_rtvHeap.GetCPUHandle(),
			currentBackBufferIndex,
			m_rtvHeap.GetDescriptorSize()
		);

		// Transition to render target
		const RenderSurface& renderSurface = m_swapChain.GetRenderSurface(currentBackBufferIndex);
		auto backBuffer = renderSurface.Resource;
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		m_cmdList.Get()->ResourceBarrier(1, &barrier);

		m_cmdList.Get()->RSSetViewports(1, &m_swapChain.GetViewport());
		m_cmdList.Get()->RSSetScissorRects(1, &m_swapChain.GetScissorRect());

		// Set render target and clear
		const float clearColor[] = { 0.2f, 0.3f, 0.4f, 1.0f }; // Dark blue
		m_cmdList.Get()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		m_cmdList.Get()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// Transition back to present
		barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);
		m_cmdList.Get()->ResourceBarrier(1, &barrier);

		m_cmdList.Close();
	}

	void Renderer::WaitForGPU()
	{
		RYU_PROFILE_SCOPE();

		DX12::Device* const device = m_device->GetDevice();
		const u32 currentBackBufferIndex = m_swapChain.GetFrameIndex();

		// Schedule a signal command in the queue
		m_cmdQueue.Signal(m_fence, m_fenceValues[currentBackBufferIndex]);

		// Wait until the fence has been crossed
		DXCallEx(m_fence.Get()->SetEventOnCompletion(m_fenceValues[currentBackBufferIndex], m_fenceEvent), device);

		if (::WaitForSingleObjectEx(m_fenceEvent, 5000, FALSE) == WAIT_TIMEOUT)
		{
			RYU_LOG_ERROR(LogRenderer, "GPU wait timed out");
		}

		// Increment the fence value for the current frame
		m_fenceValues[currentBackBufferIndex]++;
	}

	void Renderer::MoveToNextFrame()
	{
		DX12::Device* const device = m_device->GetDevice();
		const u32 currentBackBufferIndex = m_swapChain.GetFrameIndex();
		const u64 currentFenceValue = m_fenceValues[currentBackBufferIndex];

		// Schedule a signal command in the queue
		DXCallEx(m_cmdQueue.Get()->Signal(m_fence.Get(), currentFenceValue), device);

		// Update frame index
		const u32 nextFrameIndex = m_swapChain.GetFrameIndex();

		// If the next frame is not ready to be rendered yet, wait until it is ready
		if (m_fence.Get()->GetCompletedValue() < m_fenceValues[nextFrameIndex])
		{
			DXCallEx(m_fence.Get()->SetEventOnCompletion(m_fenceValues[nextFrameIndex], m_fenceEvent), device);

			if (::WaitForSingleObjectEx(m_fenceEvent, 5000, FALSE) == WAIT_TIMEOUT)
			{
				RYU_LOG_ERROR(LogRenderer, "Move to next frame timed out!");
			}
		}

		// Set the fence value for the next frame
		m_fenceValues[nextFrameIndex] = currentFenceValue + 1;
	}
}

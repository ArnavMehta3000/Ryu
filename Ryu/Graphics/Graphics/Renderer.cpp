#include "Renderer.h"
#include "Profiling/Profiling.h"
#include "Logger/Logger.h"
#include "Logger/Assert.h"

namespace Ryu::Gfx
{
	RYU_LOG_DECLARE_CATEGORY(Renderer);

	Renderer::Renderer(HWND window)
	{
		RYU_PROFILE_SCOPE();

		m_device = Device::Create();
		m_swapChain.Initialize(m_device, window, BACK_BUFFER_FORMAT);
	}
	
	Renderer::~Renderer()
	{
		RYU_PROFILE_SCOPE();

		if (m_device)
		{
			RYU_LOG_DEBUG(LogRenderer, "Waiting for GPU");
			m_device->WaitForGPU();

			RYU_LOG_DEBUG(LogRenderer, "Destroying swapchain");
			m_swapChain.Destroy();
			Device::Destroy(*m_device);

			RYU_LOG_DEBUG(LogRenderer, "Destroying graphics device");
			RYU_ASSERT(m_device.use_count() == 1);
			m_device.reset();
		}

	}

	void Renderer::Render()
	{
		RYU_PROFILE_SCOPE();

		PopulateCommandList();

		auto* cmdList = m_device->GetCommandList();
		ID3D12CommandList* ppCommandLists[] = { cmdList };
		m_device->GetCommandQueue()->ExecuteCommandLists(1, ppCommandLists);

		m_swapChain.Present();
		
		m_device->MoveToNextFrame(m_swapChain.GetFrameIndex());
	}

	void Renderer::OnResize(u32 width, u32 height)
	{
		RYU_PROFILE_SCOPE();

		m_device->WaitForGPU();
		m_swapChain.Resize(width, height);
	}
	
	void Renderer::PopulateCommandList()
	{
		RYU_PROFILE_SCOPE();

		const u32 frameIndex = m_swapChain.GetFrameIndex();
		auto* device         = m_device->GetDevice();
		auto* cmdList        = m_device->GetCommandList();
		auto* cmdAllocator   = m_device->GetCommandAllocator(frameIndex);

		// Reset command allocator and list
		DXCallEx(cmdAllocator->Reset(), device);
		DXCallEx(cmdList->Reset(cmdAllocator, nullptr), device);

		auto& rtvHeap = m_device->GetRTVDescriptorHeap();
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
			rtvHeap.GetCPUHandle(),
			frameIndex,
			rtvHeap.GetDescriptorSize()
		);

		// Transition to render target
		const RenderSurface& renderSurface = m_swapChain.GetRenderSurface(frameIndex);
		auto backBuffer = renderSurface.Resource;
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		cmdList->ResourceBarrier(1, &barrier);

		cmdList->RSSetViewports(1, &m_swapChain.GetViewport());
		cmdList->RSSetScissorRects(1, &m_swapChain.GetScissorRect());

		// Set render target and clear
		const float clearColor[] = { 0.2f, 0.3f, 0.4f, 1.0f }; // Dark blue
		cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// Transition back to present
		barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);
		cmdList->ResourceBarrier(1, &barrier);

		DXCallEx(cmdList->Close(), device);
	}
}

#include "Renderer.h"
#include "Profiling/Profiling.h"
#include "Logger/Assert.h"

namespace Ryu::Gfx
{
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
			m_swapChain.Destroy();
			Device::Destroy(*m_device);
	
			RYU_ASSERT(m_device.use_count() == 1);
			m_device.reset();
		}

	}

	void Renderer::Render()
	{
		RYU_PROFILE_SCOPE();


		//CommandList* const cmdList = m_device->GetCommandList(/*CommandListType::Direct*/);
		//cmdList->ResetCommandList();

		// Record commands

		//cmdList->CloseCommandList();
		//m_device->GetCommandContext()->ExecuteCommandList(cmdList);

		//m_swapChain->Present();
		//m_device->WaitForGPU();

		// ------------------- ^^^ NEW ^^^ --------------------------

		// ------------------- VVV OLD VVV--------------------------

		//if (auto ctx = m_device->GetCommandContext())
		//{
		//	// Wait for the GPU to finish with the command allocator and 
		//	// reset the allocator once the GPU is done with it

		//	ctx->BeginFrame();

		//	MAYBE_UNUSED DX12::GfxCmdList* const cmdList = ctx->GetGfxCmdList();

		//	// Do we want to release anything this frame?
		//	const u32 frameIndex = ctx->GetFrameIndex();
		//	if (m_device->CheckDeferredReleaseFlag(frameIndex))
		//	{
		//		m_device->ProcessDeferredReleases(frameIndex);
		//	}
		//	
		//	auto& surfaceData = m_swapChain->GetCurrentSurfaceData();
		//	
		//	// RECORD COMMANDS HERE
		//	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		//		surfaceData.Resource.Get(),
		//		D3D12_RESOURCE_STATE_PRESENT,
		//		D3D12_RESOURCE_STATE_RENDER_TARGET
		//	);
		//	cmdList->ResourceBarrier(1, &barrier);

		//	cmdList->RSSetViewports(1, &m_swapChain->GetViewport());
		//	cmdList->RSSetScissorRects(1, &m_swapChain->GetScissorRect());

		//	const f32 clearColor[] = { 1.0f, 0.2f, 0.4f, 1.0f };
		//	cmdList->ClearRenderTargetView(surfaceData.RTV.CPUHandle, clearColor, 0, nullptr);

		//	cmdList->OMSetRenderTargets(1, &surfaceData.RTV.CPUHandle, FALSE, nullptr);

		//	barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		//		surfaceData.Resource.Get(),
		//		D3D12_RESOURCE_STATE_RENDER_TARGET,
		//		D3D12_RESOURCE_STATE_PRESENT
		//	);
		//	cmdList->ResourceBarrier(1, &barrier);

		//	// Execute commands -> Signal and increment the fence value for next frame
		//	ctx->EndFrame();
		//}

		//m_swapChain->Present();
	}

	void Renderer::OnResize(u32 width, u32 height)
	{
		RYU_PROFILE_SCOPE();
		m_swapChain.Resize(width, height);
	}
}

#include "Renderer.h"
#include "Profiling/Profiling.h"
#include "Graphics/GraphicsConfig.h"
#include "Logger/Assert.h"

namespace Ryu::Gfx
{
	Renderer::Renderer(HWND window)
	{
		RYU_PROFILE_SCOPE();

		m_device    = Device::Create();
		m_swapchain = SwapChain::Create(m_device, window);
	}
	
	Renderer::~Renderer()
	{
		RYU_PROFILE_SCOPE();

		//if (m_device)
		//{
		//	m_device->GetCommandContext()->Flush();
		//}

		// Process any pending releases for all frames
		//for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		//{
		//	if (m_device)
		//	{
		//		m_device->ProcessDeferredReleases(i);
		//	}
		//}

		RYU_ASSERT(m_swapchain.use_count() == 1);
		RYU_ASSERT(m_device.use_count() == 1);

		m_swapchain.reset();
		
		Device::Destroy(*m_device);
		m_device.reset();
	}
	
	void Renderer::Render()
	{
		RYU_PROFILE_SCOPE();
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
		//	auto& surfaceData = m_swapchain->GetCurrentSurfaceData();
		//	
		//	// RECORD COMMANDS HERE
		//	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		//		surfaceData.Resource.Get(),
		//		D3D12_RESOURCE_STATE_PRESENT,
		//		D3D12_RESOURCE_STATE_RENDER_TARGET
		//	);
		//	cmdList->ResourceBarrier(1, &barrier);

		//	cmdList->RSSetViewports(1, &m_swapchain->GetViewport());
		//	cmdList->RSSetScissorRects(1, &m_swapchain->GetScissorRect());

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

		//m_swapchain->Present();
	}

	void Renderer::OnResize(u32 width, u32 height)
	{
		RYU_PROFILE_SCOPE();
		m_swapchain->Resize(width, height);
	}
}

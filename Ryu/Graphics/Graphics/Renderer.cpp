#include "Renderer.h"
#include "Profiling/Profiling.h"
#include "Graphics/GraphicsConfig.h"
#include "Logger/Assert.h"

namespace Ryu::Gfx
{
	Renderer::Renderer(HWND window)
	{
		RYU_PROFILE_SCOPE();

		m_device    = Memory::CreateRef<Device>();
		m_swapchain = Memory::CreateRef<SwapChain>(GetDevice(), window);
	}
	
	Renderer::~Renderer()
	{
		RYU_PROFILE_SCOPE();

		RYU_ASSERT(m_swapchain.GetRefCount() == 1);
		RYU_ASSERT(m_device.GetRefCount() == 1);

		m_swapchain.Reset();
		m_device.Reset();
	}
	
	void Renderer::Render()
	{
		RYU_PROFILE_SCOPE();
		if (CommandContext* ctx = m_device->GetCommandContext())
		{
			// Wait for the GPU to finish with the command allocator and 
			// reset the allocator once the GPU is done with it

			/*if (m_resizeRequested)
			{
				m_resizeRequested = false;
				m_swapchain->Resize(m_width, m_height);
			}*/
			ctx->BeginFrame();

			MAYBE_UNUSED DX12::GfxCmdList* const cmdList = ctx->GetGfxCmdList();

			// Do we want to release anything this frame?
			const u32 frameIndex = ctx->GetFrameIndex();
			if (m_device->CheckDeferredReleaseFlag(frameIndex))
			{
				m_device->ProcessDeferredReleases(frameIndex);
			}
			
			auto& surfaceData = m_swapchain->GetCurrentSurfaceData();
			
			// RECORD COMMANDS HERE
			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				surfaceData.Resource.Get(),
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			);
			cmdList->ResourceBarrier(1, &barrier);

			cmdList->RSSetViewports(1, &m_swapchain->GetViewport());
			cmdList->RSSetScissorRects(1, &m_swapchain->GetScissorRect());

			const f32 clearColor[] = { 1.0f, 0.2f, 0.4f, 1.0f };
			cmdList->ClearRenderTargetView(surfaceData.RTV.CPUHandle, clearColor, 0, nullptr);

			cmdList->OMSetRenderTargets(1, &surfaceData.RTV.CPUHandle, FALSE, nullptr);

			barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				surfaceData.Resource.Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT
			);
			cmdList->ResourceBarrier(1, &barrier);

			// Execute commands -> Signal and increment the fence value for next frame
			ctx->EndFrame();
		}

		m_swapchain->Present();
	}

	void Renderer::OnResize(u32 width, u32 height)
	{
		RYU_PROFILE_SCOPE();
		m_swapchain->Resize(width, height);
	}
}

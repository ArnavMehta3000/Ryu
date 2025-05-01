#include "Renderer.h"
#include "Profiling/Profiling.h"
#include "Graphics/GraphicsConfig.h"
#include <libassert/assert.hpp>

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

		DEBUG_ASSERT(m_swapchain.GetRefCount() == 1);
		DEBUG_ASSERT(m_device.GetRefCount() == 1);
		
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
			ctx->BeginFrame();

			MAYBE_UNUSED DX12::GfxCmdList* const cmdList = ctx->GetGfxCmdList();

			// RECORD COMMANDS HERE

			// Execute commands -> Signal and increment the fence value for nect frame
			ctx->EndFrame();
		}
	}

	void Renderer::OnResize(u32 width, u32 height)
	{
		RYU_PROFILE_SCOPE();
		m_swapchain->Resize(width, height);
	}
}

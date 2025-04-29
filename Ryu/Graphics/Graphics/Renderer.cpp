#include "Renderer.h"
#include "Profiling/Profiling.h"
#include "Graphics/GraphicsConfig.h"

namespace Ryu::Gfx
{
	Renderer::Renderer(HWND window)
	{
		RYU_PROFILE_SCOPE();
		DXCall(::CoInitializeEx(NULL, COINIT_MULTITHREADED));

		m_device    = Memory::CreateRef<Device>();
		m_swapchain = Memory::CreateRef<SwapChain>(GetDevice(), DisplayMode::SDR, GraphicsConfig::FRAME_COUNT, window);
	}
	
	Renderer::~Renderer()
	{
		RYU_PROFILE_SCOPE();
	
		::CoUninitialize();
	}
	
	void Renderer::OnResize(u32 width, u32 height)
	{
		RYU_PROFILE_SCOPE();
		m_swapchain->OnResize(width, height);
	}
}

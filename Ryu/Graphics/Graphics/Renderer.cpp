#include "Renderer.h"
#include "Profiling/Profiling.h"
#include "Graphics/GraphicsConfig.h"
#include <libassert/assert.hpp>

namespace Ryu::Gfx
{
	Renderer::Renderer(HWND window)
	{
		RYU_PROFILE_SCOPE();
		DXCall(::CoInitializeEx(NULL, COINIT_MULTITHREADED));

		m_device = std::make_unique<Device>();
		m_swapchain = std::make_unique<SwapChain>(m_device.get(), DisplayMode::SDR, GraphicsConfig::FRAME_COUNT, window);
	}
	
	Renderer::~Renderer()
	{
		RYU_PROFILE_SCOPE();

		m_swapchain.reset();
		m_device.reset();
	
		::CoUninitialize();
	}
	
	void Renderer::OnResize(u32 width, u32 height)
	{
		m_swapchain->OnResize(width, height);
	}
}

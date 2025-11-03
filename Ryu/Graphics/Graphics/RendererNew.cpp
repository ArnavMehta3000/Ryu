#include "Graphics/RendererNew.h"

namespace Ryu::Gfx
{
	RendererNew::RendererNew(HWND window)
	{
		m_device = std::make_unique<GfxDevice>(window);
		m_device->Initialize();
	}
	
	void RendererNew::Render()
	{
		m_device->BeginFrame();

		m_device->EndFrame();
	}
	
	void RendererNew::OnResize(u32 w, u32 h)
	{
		m_device->ResizeBuffers(w, h);
	}
}

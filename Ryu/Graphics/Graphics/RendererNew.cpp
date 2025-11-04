#include "Graphics/RendererNew.h"
#include "Graphics/Core/GfxTexture.h"

namespace Ryu::Gfx
{
	RendererNew::RendererNew(HWND window)
	{
		m_device = std::make_unique<Device>(window);
		m_device->Initialize();
	}
	
	void RendererNew::Render()
	{
		CommandList* cmdList  = m_device->GetGraphicsCommandList();
		Texture* renderTarget = m_device->GetCurrentBackBuffer();

		m_device->BeginFrame();

		cmdList->TransitionResource(*renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_device->SetBackBufferRenderTarget(true);

		cmdList->SetTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->TransitionResource(*renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		m_device->EndFrame();
		m_device->Present();
	}
	
	void RendererNew::OnResize(u32 w, u32 h)
	{
		m_device->ResizeBuffers(w, h);
	}
}

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
		GfxCommandList* cmdList      = m_device->GetGraphicsCommandList();
		DX12::Resource* renderTarget = m_device->GetCurrentBackBuffer();

		m_device->BeginFrame();

		cmdList->ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(
			renderTarget,
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET));

		m_device->SetBackBufferRenderTarget(true);

		cmdList->GetNative()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		cmdList->ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(
			renderTarget,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT));

		m_device->EndFrame();
		m_device->Present();
	}
	
	void RendererNew::OnResize(u32 w, u32 h)
	{
		m_device->ResizeBuffers(w, h);
	}
}

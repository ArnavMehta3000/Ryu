#pragma once
#include "GraphicsRHI/DXGI/DXGISwapChain.h"

namespace Ryu::Graphics::DX11
{
	class DX11RenderTarget;
	class DX11Device;

	class DX11SwapChain : public DXGISwapChain
	{
	public:
		DX11SwapChain(const DX11Device* device, const SwapChainDesc& desc);
		~DX11SwapChain();

	private:
		void CreateSwapChain();
		void CreateBackBufferRenderTarget();

		IRenderTarget* GetBackBufferRenderTarget() const override;

	private:
		const DX11Device* m_device;
		std::unique_ptr<DX11RenderTarget> m_renderTarget;
	};
}

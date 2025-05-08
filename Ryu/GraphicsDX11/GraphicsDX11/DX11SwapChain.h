#pragma once
#include "GraphicsDX11/DX11DeviceResource.h"
#include "GraphicsRHI/DXGI/DXGISwapChain.h"

namespace Ryu::Graphics::DX11
{
	class DX11RenderTarget;
	class DX11Device;

	class DX11SwapChain 
		: public DX11DeviceResource
		, public DXGISwapChain
	{
	public:
		DX11SwapChain(const DX11Device* device, const SwapChainDesc& desc);
		~DX11SwapChain();

	private:
		void CreateSwapChain();
		void CreateBackBufferRenderTarget();

		IRenderTarget* GetBackBufferRenderTarget() const override;

	private:
		std::unique_ptr<DX11RenderTarget> m_renderTarget;
	};
}

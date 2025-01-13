#pragma once
#include "GraphicsRHI/DXGI/DXGISwapChain.h"

namespace Ryu::Graphics::DX12
{
	class DX12RenderTarget;
	class DX12Device;

	class DX12SwapChain : public DXGISwapChain
	{
	public:
		DX12SwapChain(const DX12Device* device, const SwapChainDesc& desc);
		~DX12SwapChain();

	private:
		void CreateSwapChain();
		void CreateBackBufferRenderTarget();

		IRenderTarget* GetBackBufferRenderTarget() const override;

	private:
		const DX12Device* m_device;
	};
}

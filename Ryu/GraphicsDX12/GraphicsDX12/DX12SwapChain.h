#pragma once
#include "GraphicsDX12/DX12DeviceResource.h"
#include "GraphicsRHI/DXGI/DXGISwapChain.h"

namespace Ryu::Graphics::DX12
{
	class DX12RenderTarget;
	class DX12Device;

	class DX12SwapChain 
		: public DX12DeviceResource
		, public DXGISwapChain
	{
	public:
		DX12SwapChain(const DX12Device* device, const SwapChainDesc& desc);
		~DX12SwapChain();

	private:
		void CreateSwapChain();
		void CreateBackBufferRenderTarget();

		IRenderTarget* GetBackBufferRenderTarget() const override;

	private:
		ComPtr<DX12::IDX12DescriptorHeap> m_rtvHeap;
		std::array<DX12::IDX12Resource, 3> m_backBuffers;
	};
}
RYU_TODO("Do something about getting frame buffers from the config");

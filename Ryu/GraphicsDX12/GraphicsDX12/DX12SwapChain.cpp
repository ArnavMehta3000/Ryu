#include "DX12SwapChain.h"
#include "GraphicsRHI/Utils/D3DUtil.h"
#include "GraphicsRHI/Utils/Logging.h"
#include "GraphicsDX12/DX12Device.h"

namespace Ryu::Graphics::DX12
{
	DX12SwapChain::DX12SwapChain(const DX12Device* device, const SwapChainDesc& desc)
		: DXGISwapChain(desc)
		, m_device(device)
	{
		CreateSwapChain();
		CreateBackBufferRenderTarget();

		m_device->InitializeResource(this);
		SetName("DX12 Swap Chain");
	}

	DX12SwapChain::~DX12SwapChain() = default;
	
	void DX12SwapChain::CreateSwapChain()
	{
		m_swapChain.Reset();
	}
	
	void DX12SwapChain::CreateBackBufferRenderTarget()
	{
		
	}
	
	IRenderTarget* DX12SwapChain::GetBackBufferRenderTarget() const
	{
		return nullptr;
	}
}

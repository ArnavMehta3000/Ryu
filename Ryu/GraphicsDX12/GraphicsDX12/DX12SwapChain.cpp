#include "DX12SwapChain.h"
#include "Profiling/Profiling.h"
#include "GraphicsRHI/GraphicsConfig.h"
#include "GraphicsRHI/Utils/D3DUtil.h"
#include "GraphicsRHI/Utils/Logging.h"
#include "GraphicsDX12/DX12Device.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics::DX12
{
	RYU_LOG_CATEGORY(DX12SwapChain);

	DX12SwapChain::DX12SwapChain(const DX12Device* device, const SwapChainDesc& desc)
		: DX12DeviceResource(device)
		, DXGISwapChain(desc)
	{
		RYU_PROFILE_SCOPE();

		CreateSwapChain();
		CreateBackBufferRenderTarget();

		GetDevice()->InitializeResource(this);
		SetName("DX12 Swap Chain");
	}

	DX12SwapChain::~DX12SwapChain() = default;
	
	void DX12SwapChain::CreateSwapChain()
	{
		RYU_PROFILE_SCOPE();

		RYU_TODO("Reset back buffer render target");
		m_swapChain.Reset();

		DX12Device::NativeType* device = *GetDevice();
		IDXGIAdapter4* const adapter = GetDevice()->GetAdapter();

		u32 numerator = 60;
		u32 denominator = 1;

		const DXGI_FORMAT format = Utils::ToNonSRGBFormat(Utils::GetFormat(m_desc.Format));

		ComPtr<IDXGIOutput> output;
		for (UINT i = 0; adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			UINT numModes = 0;
			if (FAILED(output->GetDisplayModeList(format, 0, &numModes, nullptr)) || numModes == 0)
			{
				continue;
			}

			std::vector<DXGI_MODE_DESC> modes(numModes);
			if (FAILED(output->GetDisplayModeList(format, 0, &numModes, modes.data())))
			{
				continue;
			}

			for (const auto& mode : modes)
			{
				if (mode.Width == m_desc.Width &&
					mode.Height == m_desc.Height &&
					mode.Format == format)
				{
					UINT currentRate = mode.RefreshRate.Numerator / mode.RefreshRate.Denominator;
					UINT bestRate = numerator / denominator;

					if (currentRate > bestRate)
					{
						numerator = mode.RefreshRate.Numerator;
						denominator = mode.RefreshRate.Denominator;
					}
				}
			}
			output.Reset();
		}

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc1{};
		swapChainDesc1.Width              = m_desc.Width;
		swapChainDesc1.Height             = m_desc.Height;
		swapChainDesc1.Format             = format;
		swapChainDesc1.BufferCount        = m_desc.BufferCount;
		swapChainDesc1.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc1.SampleDesc.Count   = m_desc.SampleCount;
		swapChainDesc1.SampleDesc.Quality = m_desc.SampleQuality;
		swapChainDesc1.SwapEffect         = Utils::GetSwapEffect(m_desc.SwapEffect);
		swapChainDesc1.AlphaMode          = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc1.Scaling            = DXGI_SCALING_STRETCH;
		swapChainDesc1.Flags              = m_desc.Flags;
		swapChainDesc1.Stereo             = FALSE;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc{};
		fsDesc.RefreshRate.Numerator   = numerator;
		fsDesc.RefreshRate.Denominator = denominator;
		fsDesc.Windowed                = m_desc.Windowed;
		fsDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		fsDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

		ComPtr<IDXGISwapChain1> swapChain1;
		DXCall(GetDevice()->GetDXGIFactory()->CreateSwapChainForHwnd(
			device,
			m_desc.WindowHandle,
			&swapChainDesc1,
			&fsDesc,
			nullptr,  // Restrict to output window
			&swapChain1
		));

		ComPtr<IDXGISwapChain4> swapChain4;
		DXCall(swapChain1.As(&m_swapChain));
	}
	
	void DX12SwapChain::CreateBackBufferRenderTarget()
	{
		RYU_PROFILE_SCOPE();

		DEBUG_ASSERT(m_swapChain);
		DEBUG_ASSERT(m_rtvHeap);
		
		DX12Device::NativeType* device = *GetDevice();
		auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		for (u32 i = 0; i < GraphicsConfig::FRAME_COUNT; i++)
		{
			ComPtr<ID3D12Resource> backBuffer;
			DXCall(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

			device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
			m_backBuffers[i] = backBuffer;

			rtvHandle.Offset(rtvDescriptorSize);
		}
	}
	
	IRenderTarget* DX12SwapChain::GetBackBufferRenderTarget() const
	{
		RYU_PROFILE_SCOPE();
		LOG_FATAL(RYU_USE_LOG_CATEGORY(DX12SwapChain), "DX12SwapChain::GetBackBufferRenderTarget is not implemented!");
		return nullptr;
	}
}

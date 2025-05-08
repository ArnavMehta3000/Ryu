#include "DX11SwapChain.h"
#include "Profiling/Profiling.h"
#include "GraphicsRHI/Utils/D3DUtil.h"
#include "GraphicsRHI/Utils/Logging.h"
#include "GraphicsDX11/DX11RenderTarget.h"
#include "GraphicsDX11/DX11Device.h"
#include <vector>

namespace Ryu::Graphics::DX11
{
	DX11SwapChain::DX11SwapChain(const DX11Device* device, const SwapChainDesc& desc)
		: DX11DeviceResource(device)
		, DXGISwapChain(desc)
	{
		RYU_PROFILE_SCOPE();
		CreateSwapChain();
		CreateBackBufferRenderTarget();

		GetDevice()->InitializeResource(this);
		SetName("DX11 Swap Chain");
	}
	
	DX11SwapChain::~DX11SwapChain()
	{
		RYU_PROFILE_SCOPE();
		m_renderTarget.reset();
	}
	
	void DX11SwapChain::CreateSwapChain()
	{
		RYU_PROFILE_SCOPE();

		// Reset the back buffer texture and swap chain
		m_renderTarget.reset();
		m_swapChain.Reset();

		DX11Device::NativeType* device = *GetDevice();
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
				if (mode.Width  == m_desc.Width &&
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
	
	void DX11SwapChain::CreateBackBufferRenderTarget()
	{
		RYU_PROFILE_SCOPE();
		// Get native back buffer
		ComPtr<ID3D11Texture2D> nativeBackBuffer;
		DXCall(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&nativeBackBuffer)));

		ComPtr<DX11::IDX11Texture2D> nativeBackBuffer1;
		DXCall(nativeBackBuffer.As(&nativeBackBuffer1));

		// Get back buffer texture description
		D3D11_TEXTURE2D_DESC1 desc{};
		nativeBackBuffer1->GetDesc1(&desc);

		Texture2DDesc textureDesc;
		textureDesc.Width         = desc.Width;
		textureDesc.Height        = desc.Height;
		textureDesc.Format        = Utils::GetFormat(desc.Format);
		textureDesc.ArraySize     = desc.ArraySize;
		textureDesc.MipLevels     = desc.MipLevels;
		textureDesc.SampleCount   = desc.SampleDesc.Count;
		textureDesc.SampleQuality = desc.SampleDesc.Quality;
		textureDesc.Usage         = TextureUsage::ShaderResourceRT;

		auto backBufferTexture = std::make_unique<DX11Texture2D>(GetDevice(), nativeBackBuffer1.Detach());
		backBufferTexture->SetName("DX11 Back Buffer Texture");
		
		m_renderTarget = std::make_unique<DX11RenderTarget>(GetDevice(), backBufferTexture.release());
	}
	
	IRenderTarget* DX11SwapChain::GetBackBufferRenderTarget() const
	{
		return m_renderTarget.get();
	}
}

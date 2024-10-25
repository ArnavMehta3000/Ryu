#include "DX12Surface.h"
#include "App/WindowBase.h"
#include "Graphics/Config.h"
#include "Graphics/DX12/DX12Core.h"
#include "Graphics/Internal/DXInternal.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics::DX12
{
	namespace
	{
		constexpr DXGI_FORMAT ToNonSRGB(DXGI_FORMAT format)
		{
			return format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB ? DXGI_FORMAT_R8G8B8A8_UNORM : format;
		}

		bool CheckFormatSupport(DXGI_FORMAT format)
		{
			CD3DX12FeatureSupport support;
			support.Init(Core::GetDevice());

			D3D12_FORMAT_SUPPORT1 support1;
			D3D12_FORMAT_SUPPORT2 support2;
			return SUCCEEDED(support.FormatSupport(format, support1, support2));
		}
	}

	DX12Surface::DX12Surface(App::WindowBase* window)
		: ISurface(window)
		, m_swapChain(nullptr)
		, m_currentBackBufferIndex(0)
		, m_presentFlags(0)
		, m_vsync(GraphicsConfig::Get().EnableVSync)
	{
		DEBUG_ASSERT(window->GetHWND(), "Invalid window handle");
	}

	DX12Surface::~DX12Surface()
	{
		Release();
	}

	void DX12Surface::CreateSwapChain(ID3D12CommandQueue* cmdQueue, DXGI_FORMAT format)
	{
		ASSERT(cmdQueue, "Invalid factory or command queue");

		Release();

		auto* const factory = Internal::GetFactory();

		if (Internal::HasTearingSupport() && !GraphicsConfig::Get().EnableVSync)
		{
			m_presentFlags = DXGI_PRESENT_ALLOW_TEARING;
			m_allowTearing = true;
		}

		DEBUG_ASSERT(CheckFormatSupport(format), "Format not supported");

		DXGI_SWAP_CHAIN_DESC1 desc{};
		desc.AlphaMode   = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.BufferCount = BUFFER_COUNT;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.Flags       = m_allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
		desc.Format      = ToNonSRGB(format);
		desc.Width       = m_window->GetWidth();
		desc.Height      = m_window->GetHeight();
		desc.SampleDesc  = { 1, 0 };
		desc.Scaling     = DXGI_SCALING_STRETCH;
		desc.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Stereo      = false;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc{};
		if (GraphicsConfig::Get().EnableVSync)
		{
			auto [numerator, denominator] = Internal::GetRefreshRate(format, desc.Width, desc.Height);
			fsDesc.RefreshRate.Numerator = numerator;
			fsDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			fsDesc.RefreshRate.Numerator = 0;
			fsDesc.RefreshRate.Denominator = 1;
		}
		fsDesc.Windowed         = TRUE;
		fsDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
		fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		IDXGISwapChain1* swapChain{nullptr};
		DXCall(factory->CreateSwapChainForHwnd(
			cmdQueue,
			m_window->GetHWND(),
			&desc,
			nullptr,
			nullptr,
			&swapChain));

		DXCall(factory->MakeWindowAssociation(m_window->GetHWND(), DXGI_MWA_NO_ALT_ENTER));
		DXCall(swapChain->QueryInterface(IID_PPV_ARGS(&m_swapChain)));

		Graphics::Release(swapChain);

		// Get backbuffer
		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

		for (RenderTargetData& data : m_renderTargetData)
		{
			data.RTV = Core::GetRTVDescHeap().Allocate();
		}

		Finalize();
	}

	void DX12Surface::Present() const
	{
		DEBUG_ASSERT(m_swapChain);

		DXCall(m_swapChain->Present(m_vsync, m_presentFlags));
		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	}

	void DX12Surface::OnResize(u32 width, u32 height)
	{
	}

	void DX12Surface::Release()
	{
		for (RenderTargetData& data : m_renderTargetData)
		{
			Graphics::Release(data.Resource);
			Core::GetRTVDescHeap().Free(data.RTV);
		}

		Graphics::Release(m_swapChain);
	}

	void DX12Surface::Finalize()
	{
		// Create RTV for each back buffer
		for (RenderTargetData& data : m_renderTargetData)
		{
			DEBUG_ASSERT(!data.Resource);

			D3D12_RENDER_TARGET_VIEW_DESC desc{};
			desc.Format = Core::GetDefaultRenderTargetFormat();
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			
			Core::GetDevice()->CreateRenderTargetView(data.Resource, &desc, data.RTV.CPU);
		}

		DXGI_SWAP_CHAIN_DESC desc{};
		DXCall(m_swapChain->GetDesc(&desc));
		m_viewport = CD3DX12_VIEWPORT(
			0.0f, 0.0f,
			static_cast<f32>(desc.BufferDesc.Width),
			static_cast<f32>(desc.BufferDesc.Height));

		m_scissorRect = CD3DX12_RECT(
			0, 0,
			static_cast<LONG>(desc.BufferDesc.Width),
			static_cast<LONG>(desc.BufferDesc.Height));
	}
}

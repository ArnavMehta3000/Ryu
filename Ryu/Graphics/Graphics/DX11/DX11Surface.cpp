#include "DX11Surface.h"
#include "App/WindowBase.h"
#include "Graphics/Config.h"
#include "Graphics/DX11/DX11Core.h"
#include "Graphics/Internal/DXInternal.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics::DX11
{
	namespace
	{
		constexpr DXGI_FORMAT ToNonSRGB(DXGI_FORMAT format)
		{
			return format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB ? DXGI_FORMAT_R8G8B8A8_UNORM : format;
		}

		bool CheckFormatSupport(DXGI_FORMAT format)
		{
			UINT formatSupport = 0;
			if (FAILED(Core::GetDevice()->CheckFormatSupport(format, &formatSupport)))
				formatSupport = 0;

			UINT32 required = D3D11_FORMAT_SUPPORT_RENDER_TARGET | D3D11_FORMAT_SUPPORT_DISPLAY;
			if ((formatSupport & required) != required)
			{
				return false;
			}

			return true;
		}
	}

	DX11Surface::DX11Surface(App::WindowBase* window)
		: ISurface(window)
		, m_swapChain(nullptr)
		, m_currentBackBufferIndex(0)
		, m_presentFlags(0)
		, m_allowTearing(false)
		, m_vsync(GraphicsConfig::Get().EnableVSync)
	{
		DEBUG_ASSERT(window->GetHWND(), "Invalid window handle");
	}

	DX11Surface::~DX11Surface()
	{
		Release();
	}

	void DX11Surface::CreateSwapChain(DXGI_FORMAT format)
	{
		Release();

		auto* const device = Core::GetDevice();
		auto* const factory = Internal::GetFactory();

		if (Internal::HasTearingSupport() && !GraphicsConfig::Get().EnableVSync)
		{
			m_allowTearing = true;
			m_presentFlags = DXGI_PRESENT_ALLOW_TEARING;
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
			auto [numerator, denominator]  = Internal::GetRefreshRate(format, desc.Width, desc.Height);
			fsDesc.RefreshRate.Numerator   = numerator;
			fsDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			fsDesc.RefreshRate.Numerator   = 0;
			fsDesc.RefreshRate.Denominator = 1;
		}
		fsDesc.Windowed         = TRUE;
		fsDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
		fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		IDXGISwapChain1* swapChain{ nullptr };
		DXCall(factory->CreateSwapChainForHwnd(
			device,
			m_window->GetHWND(),
			&desc,
			&fsDesc,
			nullptr,
			&swapChain));

		DXCall(factory->MakeWindowAssociation(m_window->GetHWND(), DXGI_MWA_NO_ALT_ENTER));
		DXCall(swapChain->QueryInterface(IID_PPV_ARGS(&m_swapChain)));

		Graphics::Release(swapChain);

		DX11_NAME_OBJECT(m_swapChain, "Main SwapChain");

		// Get backbuffer
		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

		Finalize();
	}

	void DX11Surface::Present() const
	{
		DEBUG_ASSERT(m_swapChain);

		DXCall(m_swapChain->Present(m_vsync, m_presentFlags));
		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	}

	void DX11Surface::OnResize(u32 width, u32 height)
	{
		// TODO : Implement
		std::ignore = width;
		std::ignore = height;
		RYU_NOT_IMPLEMENTED(RYU_USE_LOG_CATEGORY(DX11Surface));
	}

	void DX11Surface::Release()
	{
		if (m_swapChain)
		{
			m_swapChain->SetFullscreenState(FALSE, NULL);
		}

		Graphics::Release(m_renderTargetData.Texture);
		Graphics::Release(m_renderTargetData.RTV);
		Graphics::Release(m_swapChain);
	}

	void DX11Surface::Finalize()
	{
		auto* const device = Core::GetDevice();

		DXCall(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&m_renderTargetData.Texture)));
		DXCall(device->CreateRenderTargetView1(m_renderTargetData.Texture, nullptr, &m_renderTargetData.RTV));
		
		DX11_NAME_OBJECT(m_renderTargetData.Texture, "Back Buffer Texture");
		DX11_NAME_OBJECT(m_renderTargetData.RTV, "Back Buffer RTV");
		
		DXGI_SWAP_CHAIN_DESC desc{};
		DXCall(m_swapChain->GetDesc(&desc));

		m_viewport = CD3D11_VIEWPORT(
			0.0f, 0.0f,
			static_cast<f32>(desc.BufferDesc.Width),
			static_cast<f32>(desc.BufferDesc.Height));

		m_scissorRect = CD3D11_RECT(
			0, 0,
			static_cast<LONG>(desc.BufferDesc.Width),
			static_cast<LONG>(desc.BufferDesc.Height));
	}

	void DX11Surface::SetBackBuffer() const
	{
		constexpr std::array<f32, 4> clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		const auto rtv = static_cast<ID3D11RenderTargetView*>(m_renderTargetData.RTV);
		DEBUG_ASSERT(rtv, "Invalid render target view");

		auto* const context = Core::GetImContext();
		context->OMSetRenderTargets(1, &rtv, nullptr);
		context->RSSetViewports(1, &m_viewport);
		context->RSSetScissorRects(1, &m_scissorRect);
		context->ClearRenderTargetView(rtv, clearColor.data());
	}
};

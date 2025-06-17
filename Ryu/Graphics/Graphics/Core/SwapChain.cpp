#include "Graphics/Core/SwapChain.h"
#include "Graphics/Core/Device.h"
#include "Graphics/GraphicsConfig.h"
#include "Profiling/Profiling.h"

namespace Ryu::Gfx
{
	static void GetWindowSize(HWND window, u32& outWidth, u32& outHeight)
	{
		if (!window)
		{
			outWidth = 0;
			outHeight = 0;
			return;
		}

		RECT r{};
		::GetClientRect(window, &r);
		outWidth = static_cast<u32>(r.right - r.left);
		outHeight = static_cast<u32>(r.bottom - r.top);
	}

	SwapChain::SwapChain(std::weak_ptr<Device> parent, HWND window, Format format)
		: DeviceObject(parent)
		, m_window(window)
		, m_format(format)
		, m_width(0)
		, m_height(0)
		, m_frameIndex(0)
	{
	}

	std::shared_ptr<SwapChain> SwapChain::Create(std::weak_ptr<Device> parent, HWND window, Format format)
	{
		auto swapChain = std::shared_ptr<SwapChain>(new SwapChain(parent, window, format));
		swapChain->CreateSwapChain();
		RYU_LOG_DEBUG(RYU_LOG_USE_CATEGORY(GFXSwapChain), "SwapChain created");
		
		return swapChain;
	}

	SwapChain::~SwapChain()
	{
		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			SurfaceData& data = m_surfaceData[i];
			data.Resource.Reset();
			GetParent()->GetRTVHeap()->Free(data.RTV);
		}
		
		m_swapChain.Reset();
	}

	void SwapChain::Resize(const u32 width, const u32 height)
	{
		RYU_PROFILE_SCOPE();

		if (!m_window || (m_width == width && m_height == height))
		{
			return;
		}

		m_width  = width;
		m_height = height;

		// Release frame buffers before resize
		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			m_surfaceData[i].Resource.Reset();
		}


		DXGI_SWAP_CHAIN_DESC1 desc{};
		m_swapChain->GetDesc1(&desc);
		DXCallEx(m_swapChain->ResizeBuffers(
			FRAME_BUFFER_COUNT,
			m_width, m_height,
			DXGI::ConvertFormat(m_format),
			desc.Flags
		), GetParent()->GetDevice());

		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			m_surfaceData[i].RTV = GetParent()->GetRTVHeap()->Allocate();
		}
		
		CreateFrameResources();

		// Set viewport and rect
		m_viewport    = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<f32>(m_width), static_cast<f32>(m_height));
		m_scissorRect = CD3DX12_RECT(0l, 0l, static_cast<LONG>(m_width), static_cast<LONG>(m_height));

		RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(GFXSwapChain), "SwapChain resized to {}x{}", m_width, m_height);
	}

	void SwapChain::Present() const
	{
		DXCallEx(m_swapChain->Present(0, 0), GetParent()->GetDevice());
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	}

	void SwapChain::CreateSwapChain()
	{
		RYU_PROFILE_SCOPE();

		if (auto device = GetParent())
		{
			DXGI::Factory* const factory = device->GetFactory();

			auto& config = GraphicsConfig::Get();
			const bool wantsTearing = config.AllowTearing;

			DXGI_SWAP_CHAIN_DESC1 desc{};
			BOOL allowTearing = FALSE;
			if (wantsTearing && SUCCEEDED(factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(BOOL))))
			{
				desc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
			}

			desc.AlphaMode   = DXGI_ALPHA_MODE_IGNORE;
			desc.BufferCount = FRAME_BUFFER_COUNT;
			desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc.Format      = DXGI::ConvertFormat(m_format);  // SRGB format not enforced
			desc.Width       = 0;
			desc.Height      = 0;
			desc.Scaling     = DXGI_SCALING_NONE;
			desc.Stereo      = FALSE;
			desc.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			desc.SampleDesc  = { 1,0 };

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc{};
			fsDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
			fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			fsDesc.Windowed         = TRUE;

			ComPtr<IDXGISwapChain1> swapChain;
			DXCall(factory->CreateSwapChainForHwnd(
				device->GetCommandContext()->GetCmdQueue(),
				m_window,
				&desc,
				&fsDesc,
				nullptr,
				&swapChain
			));

			RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(GFXSwapChain), "This SwapChain does not support fullscreen transitions");
			DXCallEx(factory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER), device->GetDevice());

			DXCallEx(swapChain.As(&m_swapChain), device->GetDevice());

			// Reuse desc width and height to get window size
			GetWindowSize(m_window, desc.Width, desc.Height);
			Resize(desc.Width, desc.Height);
		}
	}
	
	void SwapChain::CreateFrameResources()
	{
		RYU_PROFILE_SCOPE();

		DX12::Device* const device = GetParent()->GetDevice();

		// Create a RTV for each frame
		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			SurfaceData& data = m_surfaceData[i];

			DXCallEx(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&data.Resource)), device);

			D3D12_RENDER_TARGET_VIEW_DESC desc{};
			desc.Format        = DXGI::GetFormatSRGB(DXGI::ConvertFormat(DEFAULT_RTV_FORMAT));
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

			device->CreateRenderTargetView(data.Resource.Get(), &desc, data.RTV.CPUHandle);
			DX12::SetObjectName(data.Resource.Get(), std::format("Surface Resource - Frame {}", i).c_str());
		}
	}
}

#include "Graphics/SwapChain.h"
#include "Graphics/Device.h"

namespace Ryu::Gfx
{
	Format GetSwapchainFormat(DisplayMode displayMode)
	{
		switch (displayMode)
		{
		default:
		case DisplayMode::SDR:		 return Format::RGBA8_UNORM;
		case DisplayMode::HDR_PQ:	 return Format::RGB10A2_UNORM;
		case DisplayMode::HDR_scRGB: return Format::RGBA16_FLOAT;
		}
	}

	DXGI_COLOR_SPACE_TYPE GetColorSpace(DisplayMode displayMode)
	{
		switch (displayMode)
		{
		default:
		case DisplayMode::SDR:			return DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;
		case DisplayMode::HDR_PQ:		return DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
		case DisplayMode::HDR_scRGB:	return DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;
		}
	}

	SwapChain::SwapChain(Device* parent, DisplayMode displayMode, u32 frameCount, HWND window)
		: DeviceObject(parent)
		, m_window(window)
		, m_displayMode(displayMode)
		, m_format(GetSwapchainFormat(displayMode))
		, m_backBufferIndex(0)
		, m_frameCount(frameCount)
	{
		m_presentFence = std::make_shared<Fence>(parent, "Present Fence");
		RecreateSwapChain();
	}
	
	SwapChain::~SwapChain()
	{
		m_presentFence->CPUWait();
		m_swapChain->SetFullscreenState(FALSE, nullptr);
	}
	
	void SwapChain::OnResize(u32 width, u32 height)
	{
		DisplayMode desiredDisplayMode = m_displayMode;
		if (!DisplaySupportsHDR())
		{
			desiredDisplayMode = DisplayMode::SDR;
		}

		Format desiredFormat = GetSwapchainFormat(desiredDisplayMode);

		if (desiredFormat != m_format || width != m_width || height != m_height)
		{
			m_width = width;
			m_height = height;
			m_format = desiredFormat;

			m_presentFence->CPUWait();
			
			for (u64 i = 0; i < GraphicsConfig::FRAME_COUNT; i++)
			{
				RYU_TODO("Release swapchain buffers here");
			}

			DXGI_SWAP_CHAIN_DESC1 desc{};
			m_swapChain->GetDesc1(&desc);
			DXCall(m_swapChain->ResizeBuffers(
				(u32)m_backBuffers.size(),
				width, height,
				DXGI::ConvertFormat(m_format),
				desc.Flags
			));

			UINT colorSpaceSupport = 0;
			DXGI_COLOR_SPACE_TYPE colorSpace = GetColorSpace(desiredDisplayMode);
			if (SUCCEEDED(m_swapChain->CheckColorSpaceSupport(colorSpace, &colorSpaceSupport)) &&
				(colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT) == DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT)
			{
				DXCall(m_swapChain->SetColorSpace1(colorSpace));
			}
			
			for (u64 i = 0; i < GraphicsConfig::FRAME_COUNT; i++)
			{
				RYU_TODO("Recreate swapchain buffers here");
			}

			m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
		}
	}
	
	void SwapChain::Present()
	{
		auto& config = GraphicsConfig::Get();
		bool vSync = config.EnableVSync;
		bool allowTearing = config.AllowTearing;

		m_swapChain->Present(vSync ? 1 : 0, !vSync && allowTearing ? DXGI_PRESENT_ALLOW_TEARING : 0);
		m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

		// Signal and store when the GPU work for the frame we just flipped is finished.
		CmdQueue* pDirectQueue = GetParent()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_presentFence->Signal(pDirectQueue);

		::WaitForSingleObject(m_waitableObject, INFINITE);
	}
	
	void SwapChain::SetFrameCount(u32 frameCount)
	{
		m_frameCount = frameCount;
		RecreateSwapChain();
	}
	
	void SwapChain::SetMaxFrameLatency(u32 maxFrameLatency)
	{
		m_maxFrameLatency = maxFrameLatency;
		if (m_useWaitableObject)
		{
			m_swapChain->SetMaximumFrameLatency(maxFrameLatency);
		}
	}
	
	void SwapChain::SetUseWaitableSwapChain(bool enabled)
	{
		if (m_useWaitableObject != enabled)
		{
			m_useWaitableObject = enabled;
			RecreateSwapChain();
		}
	}
	
	bool SwapChain::DisplaySupportsHDR() const
	{
		ComPtr<DXGI::Output> output;
		ComPtr<DXGI::Output6> output6;

		if (SUCCEEDED(m_swapChain->GetContainingOutput(output.GetAddressOf())))
		{
			if (SUCCEEDED(output.As(&output6)))
			{
				DXGI_OUTPUT_DESC1 desc;
				if (SUCCEEDED(output6->GetDesc1(&desc)))
				{
					return desc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
				}
			}
		}

		return false;
	}
	
	void SwapChain::RecreateSwapChain()
	{
		m_presentFence->CPUWait();

		Device* device = GetParent();

		DXGI_SWAP_CHAIN_DESC1 desc{};
		BOOL allowTearing = FALSE;
		if (SUCCEEDED(device->GetFactory()->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(BOOL))))
		{
			m_AllowTearing = allowTearing;
			desc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		}
		
		if (m_useWaitableObject)
		{
			desc.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		}

		desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		desc.BufferCount = m_frameCount;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.Format = DXGI::ConvertFormat(m_format);
		desc.Width = 0;
		desc.Height = 0;
		desc.Scaling = DXGI_SCALING_NONE;
		desc.Stereo = FALSE;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.SampleDesc = { 1,0 };

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc{};
		fsDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
		fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		fsDesc.Windowed         = true;

		RYU_TODO("Clear back buffer array and resize the vector");
		
		m_swapChain.Reset();
		
		RYU_TODO("Get command queue");


		ComPtr<IDXGISwapChain1> swapChain;
		DXCall(device->GetFactory()->CreateSwapChainForHwnd(
			m_presentQueue->GetCommandQueue(),
			m_window,
			&desc,
			&fsDesc,
			nullptr,
			m_swapChain.GetAddressOf()
		));

		swapChain.As(&m_swapChain);

		if (m_waitableObject)
		{
			::CloseHandle(m_waitableObject);
			m_waitableObject = nullptr;
		}

		if (m_useWaitableObject)
		{
			m_swapChain->SetMaximumFrameLatency(m_maxFrameLatency);
			m_waitableObject = m_swapChain->GetFrameLatencyWaitableObject();
		}

		m_width = 0;
		m_height = 0;

		DXGI_SWAP_CHAIN_DESC1 descActual{};
		m_swapChain->GetDesc1(&descActual);
		OnResize(descActual.Width, descActual.Height);
	}
}

#include "Graphics/Core/SwapChain.h"
#include "Graphics/Core/Device.h"
#include "Graphics/GraphicsConfig.h"
#include "Profiling/Profiling.h"
#include "Logger/Assert.h"

namespace Ryu::Gfx
{
	RYU_LOG_DECLARE_CATEGORY(GFXSwapChain);

	namespace
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
	}

	SwapChain::SwapChain(std::weak_ptr<Device> parent, HWND window, Format format)
		: DeviceObject(parent)
		, m_window(window)
		, m_format(format)
		, m_width(0)
		, m_height(0)
		, m_frameIndex(0)
		, m_allowTearing(false)
		, m_rtvDescriptorSize(0)
	{
		OnConstruct(window, format);
	}

	SwapChain::~SwapChain()
	{
		OnDestruct();
	}

	void SwapChain::OnConstruct(HWND window, Format format)
	{
		m_window = window;
		m_format = format;
		
		GetWindowSize(m_window, m_width, m_height);
		CreateSwapChain();  // Will call Resize -> CreateFrameResources
		
		RYU_LOG_DEBUG(LogGFXSwapChain, "SwapChain created");
	}

	void SwapChain::OnDestruct()
	{
		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			//SurfaceData& data = m_surfaceData[i];
			//data.Resource.Reset();

			//if (auto parent = GetParent())
			//{
			//	parent->GetRTVHeap()->Free(data.RTV);
			//}
			//else
			//{
			//	RYU_LOG_ERROR(LogGFXSwapChain, "Parent device is null");
			//}
		}

		if (m_swapChain)
		{
			m_swapChain.Reset();
		}
	}

	void SwapChain::Resize(const u32 width, const u32 height)
	{
		RYU_PROFILE_SCOPE();

		if (!m_window || (m_width == width && m_height == height))
		{
			return;
		}

		m_width = width;
		m_height = height;

		// Release frame buffers before resize
		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			//m_surfaceData[i].Resource.Reset();
		}


		if (auto parent = GetParent())
		{
			DXGI_SWAP_CHAIN_DESC1 desc{};
			m_swapChain->GetDesc1(&desc);
			DXCallEx(m_swapChain->ResizeBuffers(
				FRAME_BUFFER_COUNT,
				m_width, m_height,
				DXGI::ToNative(m_format),
				desc.Flags
			), parent->GetDevice());

			m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

			for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
			{
				//m_surfaceData[i].RTV = parent->GetRTVHeap()->Allocate();
			}

			CreateFrameResources();

			// Set viewport and rect
			m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<f32>(m_width), static_cast<f32>(m_height));
			m_scissorRect = CD3DX12_RECT(0l, 0l, static_cast<LONG>(m_width), static_cast<LONG>(m_height));

			RYU_LOG_TRACE(LogGFXSwapChain, "SwapChain resized to {}x{}", m_width, m_height);
		}
		else
		{
			RYU_LOG_ERROR(LogGFXSwapChain, "SwapChain resize failed, parent device is null");
		}
	}

	void SwapChain::Present() const
	{
		if (auto parent = GetParent())
		{
			DXCallEx(m_swapChain->Present(0, 0), parent->GetDevice());
			m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
		}
	}

	void SwapChain::CreateSwapChain()
	{
		RYU_PROFILE_SCOPE();

		if (auto device = GetParent())
		{
			DXGI::Factory* const factory = device->GetFactory();
			DX12::CommandQueue* const cmdQueue = device->GetCommandQueue();
			if (!cmdQueue)
			{
				RYU_LOG_ERROR(LogGFXSwapChain, "Failed to create swapchain, graphics command context not available");
				return;
			}

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
			desc.Format      = DXGI::ToNative(m_format);  // SRGB format not enforced
			desc.Width       = 0;
			desc.Height      = 0;
			desc.Scaling     = DXGI_SCALING_NONE;
			desc.Stereo      = FALSE;
			desc.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			desc.SampleDesc  = { 1, 0 };

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc{};
			fsDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
			fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			fsDesc.Windowed         = TRUE;

			ComPtr<IDXGISwapChain1> swapChain;
			RYU_TODO("Create command queue for this to work");
			DXCall(factory->CreateSwapChainForHwnd(
				cmdQueue,
				m_window,
				&desc,
				&fsDesc,
				nullptr,
				&swapChain
			));

			RYU_LOG_DEBUG(LogGFXSwapChain, "This SwapChain does not support fullscreen transitions (TODO)");
			DXCallEx(factory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER), device->GetDevice());

			DXCallEx(swapChain.As(&m_swapChain), device->GetDevice());

			m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

			// Reuse desc width and height to get window size
			GetWindowSize(m_window, desc.Width, desc.Height);
			Resize(desc.Width, desc.Height);
		}
	}
	
	void SwapChain::CreateFrameResources()
	{
		if (!m_swapChain)
		{
			return;  // Why and how did this happen?
		}

		RYU_PROFILE_SCOPE();

		if (auto parent = GetParent())
		{
			if (DX12::Device* const device = parent->GetDevice())
			{
				for (u32 i = 0; i < m_surfaceData.size(); i++)
				{
					RenderSurface& surface = m_surfaceData[i];
					DescriptorHeap& heap   = parent->GetRTVDescriptorHeap();
					auto& rtvHandle        = heap.GetCPUHandle();

					RYU_ASSERT(heap.IsValid() && heap.IsInitialized(), "RTV heap is not valid/initialized");

					DXCallEx(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&surface.Resource)), device);
					
					device->CreateRenderTargetView(surface.Resource.Get(), nullptr, rtvHandle);
					rtvHandle.Offset(1, heap.GetDescriptorSize());

					DX12::SetObjectName(surface.Resource.Get(), std::format("Surface Resource - Frame {}", i).c_str());
				}
			}
		}
	}
}

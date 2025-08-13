#include "Graphics/Core/SwapChain.h"
#include "Graphics/Core/Device.h"
#include "Graphics/Core/CommandQueue.h"
#include "Graphics/Core/DescriptorHeap.h"
#include "Graphics/GraphicsConfig.h"
#include "Profiling/Profiling.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
RYU_DEBUG_BLOCK(
	namespace
	{
		void GetWindowSize(HWND window, u32& outWidth, u32& outHeight)
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
)

	SwapChain::SwapChain(std::weak_ptr<Device> parent, CommandQueue& queue, DescriptorHeap& rtvHeap, HWND window, Format format)
		: DeviceObject(parent)
		, m_window(window)
		, m_format(format)
		, m_frameIndex(0)
		, m_rtvHeap(&rtvHeap)
		, m_rtvDescriptorSize(0)
	{
		OnConstruct(queue, rtvHeap, window, format);
	}

	SwapChain::~SwapChain()
	{
		OnDestruct();
		m_rtvHeap = nullptr;
	}

	void SwapChain::OnConstruct(CommandQueue& queue, DescriptorHeap& rtvHeap, HWND window, Format format)
	{
		m_window = window;
		m_format = format;
		m_rtvHeap = &rtvHeap;

		CreateSwapChain(queue);

		RYU_LOG_DEBUG("SwapChain created");
	}

	void SwapChain::OnDestruct()
	{
		Release();
	}

	void SwapChain::Resize()
	{
		RYU_PROFILE_SCOPE();

		RYU_ASSERT(m_rtvHeap, "Descriptor heap is not initialized.");
		RYU_ASSERT(m_window, "Window is not initialized.");
		RYU_ASSERT(m_swapChain, "SwapChain is not initialized.");

		// Release old resources
		for (auto& data : m_surfaceData)
		{
			data.Resource.Reset();
		}

		DXGI_SWAP_CHAIN_DESC1 desc{};
		m_swapChain->GetDesc1(&desc); // For flags

		DXCall(m_swapChain->ResizeBuffers(
			FRAME_BUFFER_COUNT,
			0, 0,  // Dimensions
			DXGI_FORMAT_UNKNOWN,
			desc.Flags
		));

		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		// We reuse the allocated descriptors to put the new RTV's in
		Finalize();

		RYU_DEBUG_BLOCK(
			m_swapChain->GetDesc1(&desc);
			RYU_LOG_TRACE("SwapChain resized to {}x{}", desc.Width, desc.Height);
		)
	}

	void SwapChain::Present() const
	{
		RYU_ASSERT(m_swapChain, "SwapChain is not initialized!");

		if (auto parent = GetParent())
		{
			bool allowTearing = Gfx::IsTearingAllowed();
			i32 vsync         = Gfx::GetSyncInterval();

			if (allowTearing && vsync)
			{
				RYU_LOG_WARN(
					R"(VSync and AllowTearing cannot be enabled at the same time!
					Tearing will be disabled when presenting. Disable one through settings/cmdline)");

				allowTearing = false;
			}

			DXCallEx(m_swapChain->Present(
				vsync ? 1 : 0,
				allowTearing ? DXGI_PRESENT_ALLOW_TEARING : 0), parent->GetDevice());

			m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
		}
	}

	void SwapChain::CreateSwapChain(CommandQueue& queue)
	{
		RYU_ASSERT(m_rtvHeap, "Descriptor heap is not initialized.");

		RYU_PROFILE_SCOPE();

		if (auto device = GetParent())
		{
			const bool wantsTearing      = Gfx::IsTearingAllowed();
			DXGI::Factory* const factory = device->GetFactory();

			DXGI_SWAP_CHAIN_DESC1 desc{};
			BOOL allowTearing = FALSE;
			if (wantsTearing && SUCCEEDED(factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(BOOL))))
			{
				desc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
			}

			desc.AlphaMode   = DXGI_ALPHA_MODE_IGNORE;
			desc.BufferCount = FRAME_BUFFER_COUNT;
			desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc.Format      = DXGI::GetFormatNonSRGB(DXGI::ToNative(m_format));
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
				queue.Get(),
				m_window,
				&desc,
				&fsDesc,
				nullptr,
				&swapChain
			));

			RYU_LOG_DEBUG("This SwapChain does not support fullscreen transitions (TODO)");
			DXCallEx(factory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER), device->GetDevice());

			DXCallEx(swapChain.As(&m_swapChain), device->GetDevice());

			m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

			for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
			{
				m_surfaceData[i].RTV = m_rtvHeap->Allocate();
			}

			Finalize();
		}
		else
		{
			RYU_LOG_ERROR("SwapChain creation failed, parent device is null");
		}
	}

	void SwapChain::Finalize()
	{
		if (auto parent = GetParent())
		{
			DX12::Device* const device = parent->GetDevice();

			for (u32 i = 0; i < m_surfaceData.size(); i++)
			{
				RenderSurface& surface = m_surfaceData[i];

				// Ensure the resource is empty before creating the RTV
				RYU_ASSERT(!surface.Resource, "Resource is not empty!");
				DXCallEx(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&surface.Resource)), device);

				RYU_NOTE("Make the format sRGB. Currently it is using the default (swapchain) format which is linear.");
				D3D12_RENDER_TARGET_VIEW_DESC desc{};
				desc.Format        = DXGI::ToNative(m_format);
				desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

				// Expecting surfaceData RTV CPU handle to be allocated/valid
				device->CreateRenderTargetView(surface.Resource.Get(), &desc, surface.RTV.CPU);
				DX12::SetObjectName(surface.Resource.Get(), fmt::format("Surface Resource - Frame {}", i).c_str());
			}

			DXGI_SWAP_CHAIN_DESC1 desc{};
			DXCall(m_swapChain->GetDesc1(&desc));

			const u32 width  = desc.Width;
			const u32 height = desc.Height;

			RYU_DEBUG_BLOCK(
			{
				u32 windowWidth = 0, windowHeight = 0;
				GetWindowSize(m_window, windowWidth, windowHeight);
				RYU_ASSERT(width == windowWidth && height == windowHeight, "Window size does not match swapchain size.");
			})

			m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<f32>(width), static_cast<f32>(height), 0.0f, 1.0f);
			m_scissorRect = CD3DX12_RECT(0l, 0l, static_cast<LONG>(width), static_cast<LONG>(height));
		}
	}

	void SwapChain::Release()
	{
		for (u32 i = 0; i < m_surfaceData.size(); i++)
		{
			if (m_surfaceData[i].RTV.IsValid())
			{
				m_rtvHeap->Free(m_surfaceData[i].RTV);
			}
			m_surfaceData[i].Resource.Reset();
		}

		if (m_swapChain)
		{
			m_swapChain.Reset();
		}
	}
}

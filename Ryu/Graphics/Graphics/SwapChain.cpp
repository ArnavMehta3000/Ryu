#include "Graphics/SwapChain.h"
#include "Graphics/Device.h"
#include "Graphics/GraphicsConfig.h"
#include "Profiling/Profiling.h"
#include <libassert/assert.hpp>

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

	SwapChain::SwapChain(Device* parent, u32 frameCount, HWND window)
		: DeviceObject(parent)
		, m_window(window)
		, m_frameCount(frameCount)
		, m_width(0)
		, m_height(0)
		, m_frameIndex(0)
	{
		RYU_PROFILE_SCOPE();

		CreateDescriptorHeaps();
		CreateSwapChain();
		
		RYU_LOG_DEBUG(RYU_LOG_USE_CATEGORY(GFXSwapChain), "SwapChain created");
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

		// Release frame buffers before release
		for (u32 i = 0; i < m_frameCount; i++)
		{
			m_frameBuffers[i].Reset();
		}

		DXGI_SWAP_CHAIN_DESC1 desc{};
		m_swapChain->GetDesc1(&desc);
		DXCallEx(m_swapChain->ResizeBuffers(
			m_frameCount,
			m_width, m_height,
			DXGI::ConvertFormat(m_format),
			desc.Flags
		), GetParent()->GetDevice());

		CreateFrameResources();
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(GFXSwapChain), "SwapChain resized to {}x{}", m_width, m_height);
	}

	void SwapChain::CreateSwapChain()
	{
		RYU_PROFILE_SCOPE();

		Device* const device = GetParent();
		DXGI::Factory* const factory = device->GetFactory();

		auto& config = GraphicsConfig::Get();
		const bool wantsTearing = config.AllowTearing;

		DXGI_SWAP_CHAIN_DESC1 desc{};
		BOOL allowTearing = FALSE;
		if (wantsTearing && SUCCEEDED(device->GetFactory()->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(BOOL))))
		{
			desc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		}

		desc.AlphaMode   = DXGI_ALPHA_MODE_IGNORE;
		desc.BufferCount = m_frameCount;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.Format      = DXGI::ConvertFormat(m_format);
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
			device->GetCmdQueue(),
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

	void SwapChain::CreateDescriptorHeaps()
	{
		RYU_PROFILE_SCOPE();

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
		rtvHeapDesc.NumDescriptors = GraphicsConfig::FRAME_COUNT;
		rtvHeapDesc.Type           = DX12::GetDescHeapType(DescHeapType::RTV);
		rtvHeapDesc.Flags          = static_cast<D3D12_DESCRIPTOR_HEAP_FLAGS>(DescHeapFlags::None);
		
		DX12::Device* const device = GetParent()->GetDevice();
		DXCallEx(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)), device);

		m_rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(DX12::GetDescHeapType(DescHeapType::RTV));
	}
	
	void SwapChain::CreateFrameResources()
	{
		RYU_PROFILE_SCOPE();
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		DX12::Device* const device = GetParent()->GetDevice();

		// Create a RTV for each frame
		for (u32 i = 0; i < m_frameCount; i++)
		{
			DXCallEx(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_frameBuffers[i])), device);
			device->CreateRenderTargetView(m_frameBuffers[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
	}
}

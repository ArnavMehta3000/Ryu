#include "Graphics/Core/GfxSwapChain.h"
#include "Graphics/Core/GfxDevice.h"
#include "Graphics/CommandContext.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
	GfxSwapChain::GfxSwapChain(GfxDevice* device, CommandContext* context, const SwapChainConfig& config)
		: m_device(device)
		, m_context(context)
		, m_config(config)
	{
		RYU_ASSERT(m_device, "GfxDevice cannot be null");
		RYU_ASSERT(m_context, "CommandContext cannot be null");
		RYU_ASSERT(m_config.WindowHandle, "Window handle cannot be null");

		// Create RTV descriptor heap
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc
		{
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = m_config.BufferCount,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = 0
		};

		DXCall(m_device->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap)));
		DX12::SetObjectName(m_rtvHeap.Get(), "SwapChain RTV Heap");

		m_rtvDescriptorSize = m_device->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CreateSwapChain();
		CreateRenderTargets();

		RYU_LOG_INFO("GfxSwapChain created: {}x{}", m_config.Width, m_config.Height);
	}

	GfxSwapChain::~GfxSwapChain()
	{
		ReleaseRenderTargets();
		ComRelease(m_rtvHeap);
		ComRelease(m_swapChain);
	}

	void GfxSwapChain::Present()
	{
		const u32 syncInterval = m_config.EnableVSync ? 1 : 0;
		const u32 presentFlags = (!m_config.EnableVSync && m_config.AllowTearing) ? DXGI_PRESENT_ALLOW_TEARING : 0;

		DXCall(m_swapChain->Present(syncInterval, presentFlags));

		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	}

	void GfxSwapChain::Resize(u32 width, u32 height)
	{
		RYU_LOG_TRACE("SwapChain begin resize: {}x{}", width, height);

		m_config.Width = width;
		m_config.Height = height;

		// Wait for GPU to finish
		m_context->WaitForGPU();

		// Release references to swap chain buffers
		ReleaseRenderTargets();

		// Resize the swap chain
		DXGI_SWAP_CHAIN_DESC1 desc;
		m_swapChain->GetDesc1(&desc);

		DXCall(m_swapChain->ResizeBuffers(
			m_config.BufferCount,
			m_config.Width,
			m_config.Height,
			desc.Format,
			desc.Flags
		));

		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		// Recreate render target views
		CreateRenderTargets();

		RYU_LOG_DEBUG("SwapChain resized: {}x{}", width, height);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GfxSwapChain::GetCurrentRTV() const
	{
		return GetRTV(m_frameIndex);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GfxSwapChain::GetRTV(u32 index) const
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(index, m_rtvDescriptorSize);
		return handle;
	}

	void GfxSwapChain::CreateSwapChain()
	{
		DXGI_SWAP_CHAIN_DESC1 desc{};
		desc.Width              = m_config.Width;
		desc.Height             = m_config.Height;
		desc.Format             = m_config.Format;
		desc.Stereo             = FALSE;
		desc.SampleDesc.Count   = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount        = m_config.BufferCount;
		desc.Scaling            = DXGI_SCALING_NONE;
		desc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode          = DXGI_ALPHA_MODE_IGNORE;
		desc.Flags              = m_config.AllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc
		{
			.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
			.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED,
			.Windowed         = TRUE
		};

		ComPtr<IDXGISwapChain1> swapChain1;
		DXCall(m_device->GetFactory()->CreateSwapChainForHwnd(
			m_context->GetQueue(),
			m_config.WindowHandle,
			&desc,
			&fsDesc,
			nullptr,
			&swapChain1));

		DXCall(swapChain1.As(&m_swapChain));
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	}

	void GfxSwapChain::CreateRenderTargets()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		static constexpr std::array objectNames =
		{
			"Backbuffer 0",
			"Backbuffer 1",
			"Backbuffer 2",
			"Backbuffer 3"
		};
		static_assert(FRAME_BUFFER_COUNT <= objectNames.size());

		for (u32 i = 0; i < m_renderTargets.size(); i++)
		{
			DXCall(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));

			m_device->GetDevice()->CreateRenderTargetView(
				m_renderTargets[i].Get(),
				nullptr,
				rtvHandle);

			DX12::SetObjectName(m_renderTargets[i].Get(), objectNames[i]);

			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
	}

	void GfxSwapChain::ReleaseRenderTargets()
	{
		for (auto& rt : m_renderTargets)
		{
			ComRelease(rt);
		}
	}
}

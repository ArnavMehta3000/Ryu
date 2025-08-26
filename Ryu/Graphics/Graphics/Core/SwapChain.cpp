#include "Graphics/Core/SwapChain.h"
#include "Graphics/Core/Core.h"
#include "Graphics/GraphicsConfig.h"
#include "Graphics/Core/CommandContext.h"
#include "Graphics/Core/DescriptorAllocator.h"
#include "Logging/Logger.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
	SwapChain::~SwapChain()
	{
		RYU_ASSERT(!m_swapChain, "SwapChain is not properly destructed. Call Destroy() first.");
	}

	void SwapChain::Create(HWND window, DXGI_FORMAT format)
	{
		m_window = window;
		m_format = format;

		bool allowTearing = IsTearingAllowed();
		bool isVsync = GetSyncInterval();

		RYU_CODE_BLOCK("Check flags & config")
		{
			// If we want tearing then check we can tear
			if (allowTearing)
			{
				BOOL bAllowTearing = FALSE;
				Core::GetFactory()->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &bAllowTearing, sizeof(bAllowTearing));

				if (!bAllowTearing)
				{
					RYU_LOG_WARN("Present Allow Tearing is not supported on this device. Disabling tearing.");
					allowTearing = false;
				}
			}

			// We cannot have tearing with vsync
			if (allowTearing && isVsync)
			{
				RYU_LOG_WARN("Tearing is not supported with vsync enabled. Disabling tearing.");
				allowTearing = false;
			}
		}

		RYU_CODE_BLOCK("Set flags")
		{
			m_flags.set((u64)Flag::AllowTearing, allowTearing);
			m_flags.set((u64)Flag::EnableVSync, isVsync);
		}

		CreateSwapChain();
	}

	void SwapChain::Destroy()
	{
		for (auto& rtv : m_renderTargets)
		{
			ComRelease(rtv.Resource);
		}

		ComRelease(m_swapChain);
	}

	void SwapChain::CreateSwapChain()
	{
		const auto& device                    = Core::GetDevice();
		const auto& cmdQueue                  = Core::GetCommandContext().GetCommandQueue();
		DescriptorAllocator& rtvHeapAllocator = Core::GetDescriptorAllocator(DescriptorHeapType::RTV);
		DescriptorAllocator& dsvHeapAllocator = Core::GetDescriptorAllocator(DescriptorHeapType::DSV);

		RYU_CODE_BLOCK("Release existing resources")
		{
			for (auto& rtv : m_renderTargets)
			{
				ComRelease(rtv.Resource);
			}
		}

		if (m_swapChain)  // Resize
		{
			// Assume flush has been called externally

			HRESULT hr = S_OK;
			DXCall(hr = m_swapChain->ResizeBuffers(
				FRAME_BUFFER_COUNT,
				0, 0,
				m_format,
				(CheckFlag(Flag::AllowTearing)) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u
			));
		}
		else  // Create
		{
			const auto& factory = Core::GetFactory();

			DXGI_SWAP_CHAIN_DESC1 scDesc{};
			scDesc.Width              = 0;
			scDesc.Height             = 0;
			scDesc.Format             = m_format;
			scDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			scDesc.BufferCount        = FRAME_BUFFER_COUNT;
			scDesc.SampleDesc.Count   = 1;
			scDesc.SampleDesc.Quality = 0;
			scDesc.Scaling            = DXGI_SCALING_STRETCH;
			scDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			scDesc.AlphaMode          = DXGI_ALPHA_MODE_IGNORE;
			scDesc.Flags              = (CheckFlag(Flag::AllowTearing)) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc{ .Windowed = true };
			
			ComPtr<IDXGISwapChain1> sc1;
			DXCallEx(Core::GetFactory()->CreateSwapChainForHwnd(
				cmdQueue.Get(),
				m_window,
				&scDesc,
				&fsDesc,
				nullptr,
				&sc1),
			device.Get());

			DXCallEx(sc1.As(&m_swapChain), device.Get());
			DXCallEx(factory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER), device.Get());
		}

		RYU_CODE_BLOCK("Create back buffers")
		{
			for (u32 i = 0; i < m_renderTargets.size(); i++)
			{
				Surface& bb = m_renderTargets[i];
				DXCallEx(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&bb.Resource)), device.Get());
				DX12::SetObjectName(bb.Resource.Get(), fmt::format("BackbufferRT {}", i).c_str());

				D3D12_RENDER_TARGET_VIEW_DESC rtvDesc
				{
					.Format = m_format,
					.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
				};

				if (!bb.Handle.IsValid())
				{
					bb.Handle = rtvHeapAllocator.Allocate(1);
				}
				device->CreateRenderTargetView(bb.Resource.Get(), &rtvDesc, bb.Handle);
			}
		}

		m_currentFrameIndex = m_swapChain->GetCurrentBackBufferIndex();
				
		RYU_CODE_BLOCK("Set rects")
		{
			DXGI_SWAP_CHAIN_DESC1 scDesc{};
			m_swapChain->GetDesc1(&scDesc);

			m_viewport.Width    = static_cast<f32>(scDesc.Width);
			m_viewport.Height   = static_cast<f32>(scDesc.Height);
			m_viewport.TopLeftX = 0.0f;
			m_viewport.TopLeftY = 0.0f;
			m_viewport.MinDepth = D3D12_MIN_DEPTH;
			m_viewport.MaxDepth = D3D12_MAX_DEPTH;

			m_scissorRect.left   = 0;
			m_scissorRect.top    = 0;
			m_scissorRect.right  = scDesc.Width;
			m_scissorRect.bottom = scDesc.Height;
		}
	}

	void SwapChain::Present()
	{
		const bool isVsync      = CheckFlag(Flag::EnableVSync);
		const bool allowTearing = CheckFlag(Flag::AllowTearing);
		const u32 syncInterval  = isVsync ? GetSyncInterval() : 0;

		DXCall(m_swapChain->Present(
			syncInterval,
			allowTearing ? DXGI_PRESENT_ALLOW_TEARING : 0u
		));

		m_currentFrameIndex = m_swapChain->GetCurrentBackBufferIndex();
	}

	void SwapChain::Resize()
	{
		RYU_ASSERT(m_swapChain, "Tried to resize swapchain before it was created!");
		if (!m_swapChain)
		{
			RYU_LOG_ERROR("Tried to resize swapchain before it was created!");
			return;
		}
		
		CreateSwapChain();
	}
}

#include "DXGISwapChain.h"
#include <Graphics/DX12/DX12Device.h>
#include <Graphics/DX12/DX12GraphicsCommandList.h>
#include <Graphics/DX12/DX12CommandQueue.h>
#include <Graphics/DXGI/DXGIFactory.h>
#include <Graphics/DXGI/DXGIAdapter.h>
#include <Graphics/DXGI/DXGIOutput.h>
#include <Core/Utils/StringConv.h>

namespace Ryu::Graphics
{
	namespace
	{
		bool s_hasTearingSupport = false;

		DXGI_FORMAT ToNonSRGB(DXGI_FORMAT format)
		{
			return format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB ? DXGI_FORMAT_R8G8B8A8_UNORM : format;
		}
		
		DXGIOutput GetOutput(const DXGIAdapter& adapter)
		{
			ComPtr<IDXGIOutput> output;
			// Get main monitor
			if (SUCCEEDED(adapter->EnumOutputs(0, &output)))
			{
				DXGIOutput output6;
				if (SUCCEEDED(output.As(&output6)))
				{
					return output6;
				}
			}

			return DXGIOutput();
		}

		auto GetRefreshRate(const DXGIOutput& output, const DXGI_FORMAT& format, u32 width, u32 height)
		{
			u32 numModes = 0;
			if (SUCCEEDED(output->GetDisplayModeList1(format, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr)))
			{
				std::unique_ptr<DXGI_MODE_DESC1[]> modes(new DXGI_MODE_DESC1[numModes]);
				if (SUCCEEDED(output->GetDisplayModeList1(format, DXGI_ENUM_MODES_INTERLACED, &numModes, modes.get())))
				{
					for (u32 i = 0; i < numModes; i++)
					{
						if (modes[i].Width == width && modes[i].Height == height)
						{
							return std::make_pair(modes[i].RefreshRate.Numerator, modes[i].RefreshRate.Denominator);
						}
					}
				}
			}

			return std::make_pair(60u, 1u);
		}
	}

	DXGISwapChain::DXGISwapChain(InterfaceType* ptr)
		: ComPtr(ptr)
		, m_format(DXGI_FORMAT_UNKNOWN)
		, m_currentBackBufferIndex(0)
	{
	}

	DXGISwapChain::DXGISwapChain()
		: m_format(DXGI_FORMAT_UNKNOWN)
		, m_currentBackBufferIndex(0)
	{
	}

	CreateResult<DXGISwapChain> DXGISwapChain::Create(const HWND hWnd, const DX12Device& device,
		const DXGIFactory& factory, const DX12CommandQueue& cmdQueue, u32 width, u32 height, DXGI_FORMAT format, bool vsync)
	{
		RYU_GFX_ASSERT(device && factory, "Trying to create DXGISwapChain with (either) invalid DX12Device or DXGIFactory!");

		DXGISwapChain outSwapChain;
		outSwapChain.m_format = format;
		outSwapChain.m_descriptorSizes = device.GetDescriptorSizes();
		s_hasTearingSupport = factory.HasTearingSupport();

		DXGI_SWAP_CHAIN_DESC1 desc{};
		desc.Width       = width;
		desc.Height      = height;
		desc.Format      = ToNonSRGB(format);
		desc.Stereo      = FALSE;
		desc.SampleDesc  = { 1, 0 };
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = BACK_BUFFER_COUNT;
		desc.Scaling     = DXGI_SCALING_STRETCH;
		desc.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode   = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.Flags       = s_hasTearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		auto [numerator, denominator] = GetRefreshRate(GetOutput(factory.GetAdapter()), format, width, height);

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc{};
		if (vsync)
		{
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

		IDXGISwapChain1* sc{ nullptr };
		HRESULT hr = factory->CreateSwapChainForHwnd(
			cmdQueue.Get(),
			hWnd,
			&desc,
			&fsDesc,
			nullptr,
			&sc);

		if (FAILED(hr))
		{
			RYU_GFX_ASSERTHR(hr, "Failed to create DXGISwapChain!");
			return std::unexpected(hr);
		}

		RYU_GFX_ASSERTHR(hr = sc->QueryInterface(IID_PPV_ARGS(outSwapChain.ReleaseAndGetAddressOf())), "Failed to convert to IDXGISwapChain4");

		if (FAILED(factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER)))
		{
			RYU_GFX_WARN("Faile to make DX12SwapChain association (DXGI_MWA_NO_ALT_ENTER)");
		}

		outSwapChain.CreateDescriptorHeaps(device);

		RYU_GFX_DEBUG("Created DXGISwapChain");
		return outSwapChain;
	}

	void DXGISwapChain::CreateDescriptorHeaps(const DX12Device& device)
	{
		if (auto result = DX12DescriptorHeap::Create(device, DXGISwapChain::BACK_BUFFER_COUNT, D3D12_DESCRIPTOR_HEAP_TYPE_RTV); result)
		{
			m_rtvHeap = result.value();
		}
		else
		{
			RYU_GFX_FATAL("Failed to create DX12DescriptorHeap(RTV). Error: {}", HRToString(result.error()));
		}

		if (auto result = DX12DescriptorHeap::Create(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV); result)
		{
			m_dsvHeap = result.value();
		}
		else
		{
			RYU_GFX_FATAL("Failed to create DX12DescriptorHeap(DSV). Error: {}", HRToString(result.error()));
		}
	}

	void DXGISwapChain::Release()
	{
		m_rtvHeap.Reset();
		m_dsvHeap.Reset();
		m_depthStencilBuffer.Reset();
		
		for (auto& buffer : m_backBuffers)
		{
			buffer.Reset();
		}

		Reset();
	}

	void DXGISwapChain::Resize(const DX12Device& device, const DX12GraphicsCommandList& cmdList, u32 width, u32 height)
	{
		// Release all previous resources
		for (auto& buffer : m_backBuffers)
		{
			buffer.Reset();
		}
		m_depthStencilBuffer.Reset();

		{
			DXGI_SWAP_CHAIN_DESC1 desc{};
			RYU_GFX_ASSERTHR(Get()->GetDesc1(&desc), "Failed to get DXGISwapChain description");

			HRESULT hr{ S_OK };
			RYU_GFX_ASSERTHR(hr = Get()->ResizeBuffers(
				BACK_BUFFER_COUNT,
				width, height,
				desc.Format,
				desc.Flags), "Failed to resize DXGISwapChain buffers");
		}

		m_currentBackBufferIndex = 0;

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (u32 i = 0; i < BACK_BUFFER_COUNT; i++)
		{
			RYU_GFX_ASSERTHR(Get()->GetBuffer(i, IID_PPV_ARGS(m_backBuffers[i].GetAddressOf())), "Failed to get DXGISwapChain buffer");
			device->CreateRenderTargetView(m_backBuffers[i].Get(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, m_descriptorSizes.RtvDescriptorSize);
		}

		// Create depth/stencil buffer
		{
			RYU_EMIT_WARNING("Check DSV format")
			const CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
				DXGI_FORMAT_D24_UNORM_S8_UINT,
				width, height,
				1,  // Array size
				1,  // Mip levels
				1,  // Sample count
				0,  // Sample quality
				D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

			D3D12_CLEAR_VALUE clearValue{};
			clearValue.Format = desc.Format;
			clearValue.DepthStencil.Depth = 1.0f;
			clearValue.DepthStencil.Stencil = 0;

			auto props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			RYU_GFX_ASSERTHR(device->CreateCommittedResource(
				&props,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_COMMON,
				&clearValue,
				IID_PPV_ARGS(m_depthStencilBuffer.GetAddressOf())), "Failed to create depth/stencil buffer");
		}
		
		// Create depth/stencil view
		{
			RYU_EMIT_WARNING("Check DSV format")

			D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
			desc.Flags = D3D12_DSV_FLAG_NONE;
			desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;

			// Create transition
			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer.Get(),
				D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);

			device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &desc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
			cmdList->ResourceBarrier(1, &barrier);
		}
	}
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE DXGISwapChain::GetCurrentBackBufferView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
			m_currentBackBufferIndex, 
			m_descriptorSizes.RtvDescriptorSize);
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DXGISwapChain::GetDepthStencilView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
	}

	const DX12Resource& DXGISwapChain::GetCurrentBackBuffer() const
	{
		return m_backBuffers[m_currentBackBufferIndex];
	}

	void DXGISwapChain::Present()
	{
		HRESULT hr{ S_OK };
		if (FAILED(hr = Get()->Present(0, 0)))
		{
			RYU_GFX_ERROR("Failed to present DXGI swap chain. Error: {}", HRToString(hr));
		}
		
		m_currentBackBufferIndex = (m_currentBackBufferIndex + 1) % BACK_BUFFER_COUNT;
	}

}

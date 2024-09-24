#include "DXGISwapChain.h"
#include <Graphics/DX12/DX12Device.h>
#include <Graphics/DX12/DX12CommandQueue.h>
#include <Graphics/DXGI/DXGIFactory.h>

namespace Ryu::Graphics
{
	namespace
	{
		DXGI_FORMAT ToNonSRGB(DXGI_FORMAT format)
		{
			return format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB ? DXGI_FORMAT_R8G8B8A8_UNORM : format;
		}

		static bool s_hasTearingSupport = false;
	}

	DXGISwapChain::DXGISwapChain()
		: m_device(nullptr)
		, m_currentBuffer(0)
		, m_width(0)
		, m_height(0)
		, m_heapIncrement(0)
	{
	}

	DXGISwapChain::~DXGISwapChain()
	{
		Release();
	}

	void DXGISwapChain::Create(HWND hWnd, const DX12Device& device, const DXGIFactory& factory, const DX12CommandQueue& cmdQueue, u32 width, u32 height, DXGI_FORMAT format)
	{
		RYU_GFX_ASSERT(device && factory, "Trying to create DXGISwapChain with (either) invalid DX12Device or DXGIFactory!");

		m_device            = device.Get();
		m_width             = width;
		m_height            = height;
		s_hasTearingSupport = factory.HasTearingSupport();

		CreateDescriptorHeaps();

		DXGI_SWAP_CHAIN_DESC1 desc{};
		desc.Width       = m_width;
		desc.Height      = m_height;
		desc.Format      = ToNonSRGB(format);
		desc.Stereo      = FALSE;
		desc.SampleDesc  = { 1, 0 };
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = BACK_BUFFER_COUNT;
		desc.Scaling     = DXGI_SCALING_STRETCH;
		desc.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode   = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.Flags       = s_hasTearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		IDXGISwapChain1* sc;
		HRESULT hr = factory->CreateSwapChainForHwnd(
			cmdQueue.Get(),
			hWnd,
			&desc,
			nullptr,
			nullptr,
			&sc);

		if (FAILED(hr))
		{
			RYU_GFX_ASSERTHR(hr, "Failed to create DXGISwapChain!");
			return;
		}

		if (FAILED(hr = factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER)))
		{
			RYU_GFX_WARN("Faile to make DX12SwapChain association (DXGI_MWA_NO_ALT_ENTER)");
		}

		RYU_GFX_ASSERTHR(hr = sc->QueryInterface(IID_PPV_ARGS(ReleaseAndGetAddressOf())), "Failed to convert to IDXGISwapChain4");
		CreateBuffers();
	}

	void DXGISwapChain::Present()
	{
		Get()->Present(0, s_hasTearingSupport ? DXGI_PRESENT_ALLOW_TEARING : 0);
		m_currentBuffer = (m_currentBuffer + 1) % BACK_BUFFER_COUNT;
	}

	void DXGISwapChain::Release()
	{
		if (Get())
		{
			Reset();
			ReleaseBuffers();
			m_rtvHeap.Reset();
		}
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DXGISwapChain::GetCurrentRTVHandle() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_currentBuffer, m_heapIncrement);
	}

	void DXGISwapChain::ReleaseBuffers()
	{
		for (u32 i = 0; i < BACK_BUFFER_COUNT; i++)
		{
			m_renderTargets[i].Reset();
		}
	}

	void DXGISwapChain::CreateDescriptorHeaps()
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = m_renderTargets.size();
		desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask       = 0;

		HRESULT hr = S_OK;
		RYU_GFX_ASSERTHR(hr = m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_rtvHeap.ReleaseAndGetAddressOf())), "Failed to create ID3D12DescriptorHeap");
		m_heapIncrement = m_device->GetDescriptorHandleIncrementSize(desc.Type);
	}

	void DXGISwapChain::CreateBuffers()
	{
		RYU_GFX_ASSERT(Get(), "Trying to create buffers on an invalid DXGISwapChain");

		HRESULT hr = S_OK;
		for (u32 i = 0; i < BACK_BUFFER_COUNT; i++)
		{
			auto& rt = m_renderTargets[i];
			
			rt.Reset();
			RYU_GFX_ASSERTHR(hr = Get()->GetBuffer(i, IID_PPV_ARGS(rt.ReleaseAndGetAddressOf())), "Failed to create DXGISwapChain buffer at index {}", i);
		
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), i, m_heapIncrement);
			m_device->CreateRenderTargetView(rt.Get(), 0, cpuHandle);
		}
	}
}

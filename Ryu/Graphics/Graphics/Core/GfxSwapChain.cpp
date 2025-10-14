#include "Graphics/Core/GfxSwapChain.h"
#include "Graphics/Core/GfxDevice.h"
#include "Graphics/Core/GfxCommandList.h"
#include "Graphics/Core/GfxCommandQueue.h"

namespace Ryu::Gfx
{
	GfxSwapChain::GfxSwapChain(GfxDevice* device, const GfxSwapChain::Desc& desc)
		: m_device(device)
		, m_width(desc.Width)
		, m_height(desc.Height)
		, m_backBufferIndex(0)
	{
		DXGI_SWAP_CHAIN_DESC1 scDesc{};
		scDesc.AlphaMode          = DXGI_ALPHA_MODE_IGNORE;
		scDesc.BufferCount        = FRAME_BUFFER_COUNT;
		scDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.Format             = desc.BackBufferFormat;
		scDesc.Width              = m_width;
		scDesc.Height             = m_height;
		scDesc.Scaling            = DXGI_SCALING_NONE;
		scDesc.Stereo             = FALSE;
		scDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scDesc.SampleDesc.Count   = 1;
		scDesc.SampleDesc.Quality = 0;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc
		{
			.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
			.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED,
			.Windowed         = TRUE
		};

		DX12::Device* dx12Device  = device->GetDevice();
		GfxCommandQueue& cmdQueue = device->GetGraphicsCommandQueue();

		ComPtr<IDXGISwapChain1> swapChain1;
		DXCallEx(device->GetFactory()->CreateSwapChainForHwnd(
			cmdQueue,
			device->GetHWND(),
			&scDesc,
			&fsDesc,
			nullptr,
			&swapChain1),
			dx12Device);
		
		m_swapChain.Reset();
		DXCallEx(swapChain1.As(&m_swapChain), dx12Device);

		DXCall(device->GetFactory()->MakeWindowAssociation(device->GetHWND(), DXGI_MWA_NO_ALT_ENTER));

		m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
		CreateBackBuffers();
	}
	
	void GfxSwapChain::SetAsRenderTarget(GfxCommandList* /*cmdList*/)
	{
		RYU_NOT_IMPLEMENTED();
	}
	
	void GfxSwapChain::ClearBackBuffer(GfxCommandList* /*cmdList*/)
	{
		RYU_NOT_IMPLEMENTED();
	}
	
	void GfxSwapChain::Present(bool vsync)
	{
		DXCall(m_swapChain->Present(vsync, 0));
		m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	}
	
	void GfxSwapChain::OnResize(u32 width, u32 height)
	{
		m_width = width;
		m_height = height;

		for (auto& buffer : m_backBuffers)
		{
			buffer.reset(nullptr);
		}

		DXGI_SWAP_CHAIN_DESC desc{};
		m_swapChain->GetDesc(&desc);

		DXCall(m_swapChain->ResizeBuffers(desc.BufferCount, width, height, desc.BufferDesc.Format, desc.Flags));
		m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

		CreateBackBuffers();
	}
	
	void GfxSwapChain::CreateBackBuffers()
	{
		static std::array<f32, 4> clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		for (u32 i = 0; i < m_backBuffers.size(); i++)
		{
			DX12::Resource* backBuffer = nullptr;
			DXCall(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

			D3D12_RESOURCE_DESC resDesc = backBuffer->GetDesc();

			GfxTexture::Desc texDesc;
			texDesc.Width        = (u32)resDesc.Width;
			texDesc.Height       = resDesc.Height;
			texDesc.Format       = resDesc.Format;
			texDesc.MipLevels    = resDesc.MipLevels;
			texDesc.ArraySize    = resDesc.DepthOrArraySize;
			texDesc.SampleCount  = resDesc.SampleDesc.Count;
			texDesc.InitialState = D3D12_RESOURCE_STATE_PRESENT;
			texDesc.ClearValue   = CD3DX12_CLEAR_VALUE(texDesc.Format, clearColor.data());
			texDesc.BindFlags    = TextureFlags::RenderTarget;

			m_backBuffers[i] = m_device->CreateBackBufferTexture(texDesc, backBuffer);
			m_backBuffers[i]->SetName(fmt::format("Backbuffer {}", i).c_str());

			// Transition this to make use of the device->CreateTextureRTV()
			// For now do everything here

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc
			{
				.Format        = texDesc.Format,
				.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
			};

			GfxDescriptor descriptor = m_device->AllocateDescriptorCPU(DescriptorHeapType::RTV);
			m_device->GetDevice()->CreateRenderTargetView(m_backBuffers[i]->GetNative().Get(), &rtvDesc, descriptor);
		}
	}
	
	GfxDescriptor GfxSwapChain::GetBackBufferDescriptor() const
	{
		return m_backBufferRTVs[m_backBufferIndex];
	}
}

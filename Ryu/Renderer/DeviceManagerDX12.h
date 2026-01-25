#pragma once
#include "Renderer/DeviceManager.h"

#include <dxgi1_5.h>
#include <dxgidebug.h>
#include <nvrhi/d3d12.h>
#include <nvrhi/validation.h>

namespace Ryu::Graphics
{
	class DeviceManagerDX12 final : public DeviceManager
	{
	public:
		DeviceManagerDX12();

		std::string GetAdapterName(const DXGI_ADAPTER_DESC& desc);


	public:  // Interface
		API GetAPI() const override { return API::DirectX12; }
		bool EnumerateAdapters(std::vector<AdapterInfo>& outAdapters) override;
		nvrhi::IDevice* GetDevice() const override { return m_nvrhiDevice; }
		std::string_view GetRendererName() const { return m_rendererString; }
		nvrhi::ITexture* GetCurrentBackBuffer();
		nvrhi::ITexture* GetBackBuffer(u32 index);
		u32 GetCurrentBackBufferIndex();
		u32 GetBackBufferCount();
		void ReportLiveObjects() override;

	protected:
		bool CreateInstanceInternal() override;
		bool CreateDevice() override;
		bool CreateSwapChain() override;
		void DestroyDeviceAndSwapChain() override;
		void ResizeSwapChain() override;
		bool BeginFrame() override;
		bool Present() override;
		void Shutdown() override;

	private:
		bool CreateRenderTargets();
		void ReleaseRenderTargets();

	private:
		nvrhi::RefCountPtr<IDXGIFactory2>               m_dxgiFactory;
		nvrhi::RefCountPtr<ID3D12Device>                m_device;
		nvrhi::RefCountPtr<ID3D12CommandQueue>          m_graphicsQueue;
		nvrhi::RefCountPtr<ID3D12CommandQueue>          m_computeQueue;
		nvrhi::RefCountPtr<ID3D12CommandQueue>          m_copyQueue;
		nvrhi::RefCountPtr<IDXGISwapChain3>             m_swapChain;
		nvrhi::RefCountPtr<IDXGIAdapter>                m_dxgiAdapter;
		DXGI_SWAP_CHAIN_DESC1                           m_swapChainDesc{};
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC                 m_fullScreenDesc{};
		std::vector<nvrhi::RefCountPtr<ID3D12Resource>> m_swapChainBuffers;
		std::vector<nvrhi::TextureHandle>               m_rhiSwapChainBuffers;
		nvrhi::RefCountPtr<ID3D12Fence>                 m_frameFence;
		std::vector<HANDLE>                             m_frameFenceEvents;
		u64                                             m_frameCount = 1;
		nvrhi::DeviceHandle                             m_nvrhiDevice;
		std::string                                     m_rendererString;
		bool                                            m_isTearingSupported = false;
	};
}

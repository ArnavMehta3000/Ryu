#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	class GfxDevice;
	class CommandContext;

	struct SwapChainConfig
	{
		u32 Width          = 0;
		u32 Height         = 0;
		HWND WindowHandle  = nullptr;
		DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		u32 BufferCount    = FRAME_BUFFER_COUNT;
		bool AllowTearing  = false;
		bool EnableVSync   = true;
	};

	class GfxSwapChain
	{
	public:
		GfxSwapChain(GfxDevice* device, CommandContext* context, const SwapChainConfig& config);
		~GfxSwapChain();

		RYU_DISABLE_COPY(GfxSwapChain)

		[[nodiscard]] inline auto GetSwapChain() const noexcept { return m_swapChain.Get(); }
		[[nodiscard]] inline auto GetCurrentBackBuffer() const noexcept { return m_renderTargets[m_frameIndex].Get(); }
		[[nodiscard]] inline auto GetBackBuffer(u32 index) const noexcept { return m_renderTargets[index].Get(); }
		[[nodiscard]] inline u32 GetCurrentFrameIndex() const noexcept { return m_frameIndex; }
		[[nodiscard]] inline DXGI_FORMAT GetFormat() const noexcept { return m_config.Format; }
		[[nodiscard]] inline u32 GetWidth() const noexcept { return m_config.Width; }
		[[nodiscard]] inline u32 GetHeight() const noexcept { return m_config.Height; }

		void Present();
		void Resize(u32 width, u32 height);

		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetRTV(u32 index) const;

	private:
		void CreateSwapChain();
		void CreateRenderTargets();
		void ReleaseRenderTargets();

	private:
		GfxDevice*                    m_device = nullptr;
		CommandContext*               m_context = nullptr;
		SwapChainConfig               m_config;

		ComPtr<DXGI::SwapChain>       m_swapChain;
		ComFrameArray<DX12::Resource> m_renderTargets;
		ComPtr<DX12::DescriptorHeap>  m_rtvHeap;
		u32                           m_rtvDescriptorSize = 0;
		u32                           m_frameIndex = 0;
	};
}

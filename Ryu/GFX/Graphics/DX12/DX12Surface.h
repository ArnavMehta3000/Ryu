#pragma once
#include "Graphics/ISurface.h"
#include "Graphics/DX12/DX12Resources.h"
#include <array>

namespace Ryu::Graphics::DX12
{
	class DX12Surface : public ISurface
	{
	public:
		explicit DX12Surface(App::WindowBase* window);
		~DX12Surface();

		void CreateSwapChain(IDXGIFactory7* factory, ID3D12CommandQueue* cmdQueue, DXGI_FORMAT format);
		
		void Present() const override;
		void OnResize(u32 width, u32 height) override;
		constexpr inline u32 GetWidth() const noexcept override { return u32(m_viewport.Width); }
		constexpr inline u32 GetHeight() const noexcept override { return u32(m_viewport.Height); }
		ID3D12Resource* const GetBackBuffer() const noexcept { return m_renderTargetData[m_currentBackBufferIndex].Resource; }

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetRTV() const noexcept { return m_renderTargetData[m_currentBackBufferIndex].RTV.CPU; }
		const CD3DX12_VIEWPORT& GetViewport() const noexcept { return m_viewport; }
		const CD3DX12_RECT& GetScissorRect() const noexcept { return m_scissorRect; }

	private:
		void Release();
		void Finalize();

	public:
		static constexpr u32 BUFFER_COUNT = 3;

	private:
		struct RenderTargetData
		{
			ID3D12Resource* Resource = nullptr;
			DescriptorHandle RTV{};
		};

		IDXGISwapChain4*                           m_swapChain;
		mutable u32                                m_currentBackBufferIndex;
		std::array<RenderTargetData, BUFFER_COUNT> m_renderTargetData{};
		CD3DX12_VIEWPORT                           m_viewport;
		CD3DX12_RECT                               m_scissorRect;
		u32                                        m_presentFlags;
		u32                                        m_allowTearing;
	};
}

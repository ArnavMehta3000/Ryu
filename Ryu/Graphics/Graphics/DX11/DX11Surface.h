#pragma once
#include "Graphics/Common.h"
#include "Graphics/ISurface.h"

namespace Ryu::Graphics::DX11
{
	class DX11Surface : public ISurface
	{
		struct RenderTargetData;
	public:
		explicit DX11Surface(App::WindowBase* window);
		~DX11Surface();

		void CreateSwapChain(DXGI_FORMAT format);

		void Present() const override;
		void OnResize(u32 width, u32 height) override;
		constexpr inline u32 GetWidth() const noexcept override { return u32(m_viewport.Width); }
		constexpr inline u32 GetHeight() const noexcept override { return u32(m_viewport.Height); }
		
		const RenderTargetData& GetRenderTargetData() const noexcept { return m_renderTargetData; }
		const CD3D11_VIEWPORT& GetViewport() const noexcept { return m_viewport; }
		const CD3D11_RECT& GetScissorRect() const noexcept { return m_scissorRect; }

		void SetBackBuffer() const;

	private:
		void Release();
		void Finalize();

	public:
		static constexpr u32 BUFFER_COUNT = 3;

	private:
		struct RenderTargetData
		{
			ID3D11RenderTargetView1* RTV{ nullptr };
			ID3D11Texture2D1* Texture{ nullptr };
		};

		IDXGISwapChain4*  m_swapChain;
		mutable u32       m_currentBackBufferIndex;
		RenderTargetData  m_renderTargetData;
		CD3D11_VIEWPORT   m_viewport;
		CD3D11_RECT       m_scissorRect;
		u32               m_presentFlags;
		u32               m_allowTearing;
		bool              m_vsync;
	};
}

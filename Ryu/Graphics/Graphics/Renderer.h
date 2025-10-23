#pragma once
#include "Graphics/Core/DX12.h"
#include "Graphics/Core/GfxDevice.h"
#include "Graphics/Core/GfxSwapChain.h"
#include "Graphics/Core/GfxBuffer.h"
#include "Graphics/CommandContext.h"
#include <memory>

namespace Ryu::Gfx
{
	class Renderer
	{
	public:
		Renderer(HWND window);
		~Renderer();

		void Render();
		void OnResize(u32 w, u32 h);

	private:
		void CreatePipelineState();
		void CreateVertexBuffer();

	private:
		HWND                            m_hWnd = nullptr;
		u32                             m_width = 0;
		u32                             m_height = 0;

		std::unique_ptr<GfxDevice>      m_device;
		std::unique_ptr<CommandContext> m_context;
		std::unique_ptr<GfxSwapChain>   m_swapChain;
		std::unique_ptr<GfxBuffer>      m_vertexBuffer;

		ComPtr<DX12::RootSignature>     m_rootSig;
		ComPtr<DX12::PipelineState>     m_pipelineState;

		CD3DX12_VIEWPORT                m_viewport;
		CD3DX12_RECT                    m_scissorRect;
	};
}

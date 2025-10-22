#pragma once
#include "Graphics/Core/DX12.h"

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
		void CreateDevice();
		void CreateCommandQueue();
		void CreateSwapChain();
		void CreateDescriptorHeaps();
		void CreateFrameResources();
		void CreateCmdList();
		void CreateSynchronization();
		void WaitForPreviousFrame();

	private:
		HWND m_hWnd = nullptr;
		u32 m_width = 0;
		u32 m_height = 0;

		ComPtr<DX12::Device>               m_device;
		ComPtr<DXGI::Factory>              m_factory;
		ComPtr<DXGI::SwapChain>            m_swapChain;
		
		FrameArray<ComPtr<DX12::Resource>> m_renderTargets;
		ComPtr<DX12::DescriptorHeap>       m_rtvHeap;
		ComPtr<DX12::GraphicsCommandList>  m_cmdList;
		ComPtr<DX12::CommandQueue>         m_cmdQueue;
		ComPtr<DX12::CommandAllocator>     m_cmdAllocator;
		u32                                m_descriptorSize = 0;

		u32                                m_frameIndex = 0;
		HANDLE                             m_fenceEvent = nullptr;
		ComPtr<DX12::Fence>                m_fence;
		u64                                m_fenceValue = 0;

		CD3DX12_VIEWPORT                   m_viewport;
		CD3DX12_RECT                       m_scissorRect;
	};
}

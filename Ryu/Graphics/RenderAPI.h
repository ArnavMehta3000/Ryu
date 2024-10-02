#pragma once
#include <Graphics/DX12/DX12Device.h>
#include <Graphics/DX12/DX12Fence.h>
#include <Graphics/DX12/DX12CommandQueue.h>
#include <Graphics/DX12/DX12GraphicsCommandList.h>
#include <Graphics/DXGI/DXGISwapChain.h>

namespace Ryu::Graphics
{
	class RenderAPI
	{
	public:
		RenderAPI();
		bool Initialize(HWND hWnd, u32 width, u32 height);
		void Shutdown();
		void Present();
		void OnResize(u32 width, u32 height);

	private:
		void FlushCommandQueue();

	private:
		HWND                    m_hWnd;
		DX12Device              m_device;
		DX12CommandQueue        m_commandQueue;
		DX12GraphicsCommandList m_commandList;
		DXGISwapChain           m_swapChain;
		DX12Fence               m_fence;
		u64                     m_currentFence;
		u32                     m_width;
		u32                     m_height;
		CD3DX12_RECT            m_scissorRect;
		CD3DX12_VIEWPORT        m_viewport;
	};
}


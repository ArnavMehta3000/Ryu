#pragma once
#include <Graphics/DX12/DX12Device.h>
#include <Graphics/DX12/DX12CommandQueue.h>
#include <Graphics/DX12/DX12CommandList.h>
#include <Graphics/DXGI/DXGISwapChain.h>

namespace Ryu::Graphics
{
	class RenderAPI
	{
	public:
		RenderAPI();
		~RenderAPI();
		bool Initialize(HWND hWnd, u32 width, u32 height);
		void Shutdown();

	private:
		DX12Device m_device;
		DX12CommandQueue m_commandQueue;
		DX12CommandList m_commandList;
		DXGISwapChain m_swapChain;
	};
}


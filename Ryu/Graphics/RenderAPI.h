#pragma once
#include <Graphics/DX12/DX12Device.h>
#include <Graphics/DX12/DX12CommandQueue.h>
#include <Graphics/DX12/DX12CommandList.h>

namespace Ryu::Graphics
{
	class RenderAPI
	{
	public:
		RenderAPI();
		~RenderAPI();
		bool Initialize(HWND hWnd);
		void Shutdown();

	private:
		DX12Device m_device;
		DX12CommandQueue m_commandQueue;
		DX12CommandList m_commandList;
	};
}


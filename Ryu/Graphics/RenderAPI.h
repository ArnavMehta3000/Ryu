#pragma once
#include <Graphics/DX12/DX12Device.h>
#include <Graphics/DX12/DX12CommandQueue.h>

namespace Ryu::Graphics
{
	class RYU_API RenderAPI
	{
	public:
		RenderAPI();
		~RenderAPI();
		bool Initialize(HWND hWnd);
		void Shutdown();

	private:
		std::unique_ptr<DX12Device> m_device;
		DX12CommandQueue m_commandQueue;
	};
}


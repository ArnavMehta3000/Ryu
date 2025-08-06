#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	class SwapChain2
	{
	public:
		~SwapChain2();

		void Create(DX12::Device* device);
		void Destroy();

	private:
		HWND m_window = nullptr;
		ComPtr<DXGI::SwapChain> m_swapChain;
	};
}

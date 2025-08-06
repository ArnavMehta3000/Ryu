#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx::Core
{
	void Init(HWND window, DXGI_FORMAT backBufferFormat);
	void Shutdown();

	const CD3DX12FeatureSupport& GetFeatureSupport();
	const ComPtr<DX12::Device>& GetDevice();
	const ComPtr<DXGI::Factory>& GetFactory();
	const ComPtr<DXGI::SwapChain&> GetSwapChain();
}

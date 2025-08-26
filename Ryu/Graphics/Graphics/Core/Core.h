#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx::Core
{
	void Init(HWND window);
	void Shutdown();

	void Render();
	void Resize(u32 width, u32 height);

	const CD3DX12FeatureSupport& GetFeatureSupport();
	const ComPtr<DX12::Device>& GetDevice();
	const ComPtr<DXGI::Factory>& GetFactory();
	const SwapChain& GetSwapChain();
	CommandContext& GetCommandContext();
	DescriptorHeap& GetShaderVisibleHeap();
	DescriptorAllocator& GetDescriptorAllocator(DescriptorHeapType type);
}

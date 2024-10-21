#pragma once
#include "Graphics/Common.h"

namespace Ryu::App { class WindowBase; }
namespace Ryu::Graphics::DX12 { class DescriptorHeap; }
namespace Ryu::Graphics { class ISurface; }

namespace Ryu::Graphics::DX12::Core
{
	bool Init();
	void Shutdown();

	ID3D12Device8* const GetDevice();

	DXGI_FORMAT GetDefaultRenderTargetFormat();
	u32 GetCurrentFrameIndex();
	void SetDeferredReleaseFlag();
	DescriptorHeap& GetRTVDescHeap();
	DescriptorHeap& GetDSVDescHeap();
	DescriptorHeap& GetSRVDescHeap();
	DescriptorHeap& GetUAVDescHeap();

	ISurface* CreateSurface(App::WindowBase* window);
	void OnResizeSurface(u32 width, u32 height);
	void RenderSurface();

	namespace Internal
	{
		void DeferredRelease(IUnknown* ptr);
	}

	template <typename T>
	constexpr void DeferredRelease(T*& ptr)
	{
		if (ptr)
		{
			Internal::DeferredRelease(ptr);
			ptr = nullptr;
		}
	}


	void CreateRootSignature();
}

#pragma once
#include "Graphics/Common.h"

namespace Ryu::Graphics::DX12::Core
{
	bool Init();
	void Shutdown();
	void Render();

	ID3D12Device8* GetDevice();

	u32 GetCurrentFrameIndex();
	void SetDeferredReleaseFlag();

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
}

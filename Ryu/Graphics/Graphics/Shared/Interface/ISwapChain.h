#pragma once
#include "Common/StandardTypes.h"
#include "Graphics/Shared/SharedTypes.h"
#include "Graphics/Shared/Interface/IGraphicsObject.h"

namespace Ryu::Graphics
{
	struct SwapChainDesc
	{
		u32 Width;
		u32 Height;
		u32 BufferCount;
		u32 SampleCount;
		u32 SampleQuality;
		u32 Flags;
		Format Format;
		SwapEffect SwapEffect;
		HWND WindowHandle;
		bool Windowed;
	};

	class ISwapChain : public IGraphicsObject
	{
	public:
		virtual ~ISwapChain() = default;
		virtual void Present() = 0;
		virtual void Resize(u32 width, u32 height) = 0;
		virtual u32 GetCurrentBackBufferIndex() const = 0;
	};
}

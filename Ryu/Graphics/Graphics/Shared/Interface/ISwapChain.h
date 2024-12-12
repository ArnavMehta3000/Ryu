#pragma once
#include "Common/StandardTypes.h"
#include "Graphics/Shared/SharedTypes.h"

namespace Ryu::Graphics
{
	struct SwapChainDesc
	{
		u32 Width;
		u32 Height;
		u32 BufferCount;
		Format Format;
		SwapEffect SwapEffect;
	};

	class ISwapChain
	{
	public:
		virtual ~ISwapChain() = default;
		virtual void Present() = 0;
		virtual void Resize(u32 width, u32 height) = 0;
		virtual u32 GetCurrentBackBufferIndex() const = 0;
	};
}

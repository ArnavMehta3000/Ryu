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
		ISwapChain(const SwapChainDesc& desc) : m_desc(desc) {}

		virtual ~ISwapChain() = default;
		virtual void Present() = 0;
		virtual void Resize(u32 width, u32 height) = 0;
		virtual u32 GetCurrentBackBufferIndex() const = 0;
		virtual u32 GetCurrentFrameIndex() const = 0;
		
		inline const SwapChainDesc& GetDesc() const { return m_desc; }

	protected:
		SwapChainDesc m_desc;
	};
}

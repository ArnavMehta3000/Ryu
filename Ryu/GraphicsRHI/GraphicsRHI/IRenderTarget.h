#pragma once
#include "Common/StandardTypes.h"
#include "GraphicsRHI/SharedTypes.h"
#include "GraphicsRHI/IResource.h"

namespace Ryu::Graphics
{
	class ITexture2D;

	class IRenderTarget: public IResource
	{
	public:
		virtual ~IRenderTarget() = default;
		virtual Format GetFormat() const = 0;
		virtual u32 GetWidth() const = 0;
		virtual u32 GetHeight() const = 0;
		virtual ITexture2D* GetTexture() const = 0;
	};
}

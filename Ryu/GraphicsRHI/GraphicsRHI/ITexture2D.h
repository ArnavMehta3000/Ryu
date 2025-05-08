#pragma once
#include "Common/StandardTypes.h"
#include "GraphicsRHI/ITexture.h"

namespace Ryu::Graphics
{
	struct Texture2DDesc
	{
		u32 Width          = 0;
		u32 Height         = 0;
		u32 MipLevels      = 1;
		u32 ArraySize      = 1;
		u32 SampleCount    = 1;
		u32 SampleQuality  = 0;
		Format Format      = Format::Unknown;
		TextureUsage Usage = TextureUsage::None;
	};

	class ITexture2D : public ITexture
	{
	public:
		virtual ~ITexture2D() = default;
		virtual u32 GetWidth() const = 0;
		virtual u32 GetHeight() const = 0;
	};
}

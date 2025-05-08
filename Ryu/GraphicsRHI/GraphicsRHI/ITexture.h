#pragma once
#include "GraphicsRHI/IResource.h"
#include "GraphicsRHI/SharedTypes.h"

namespace Ryu::Graphics
{
	class ITexture : public IResource
	{
	public:
		virtual ~ITexture() = default;
		virtual Format GetFormat() const = 0;
		virtual TextureUsage GetUsage() const = 0;
	};
}

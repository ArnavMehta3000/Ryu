#pragma once
#include "GraphicsRHI/IGraphicsObject.h"

namespace Ryu::Graphics
{
	class IResource : public IGraphicsObject
	{
	public:
		virtual ~IResource() = default;
	};
}

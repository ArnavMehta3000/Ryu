#pragma once
#include "Renderer/GraphicsAPI.h"

namespace Ryu::Graphics
{
	class DeviceManager
	{
	public:
		static DeviceManager* Create(GraphicsAPI api);
	};
}

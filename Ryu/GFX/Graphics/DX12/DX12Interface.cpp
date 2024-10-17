#include "DX12Interface.h"
#include "Graphics/GraphicsAPI.h"
#include "Graphics/DX12/DX12Core.h"

namespace Ryu::Graphics::DX12
{
	void GetGraphicsInterface(GraphicsInterface& gfx)
	{
		gfx.Init = Core::Init;
		gfx.Shutdown = Core::Shutdown;
		gfx.Render = Core::Render;
	}
}

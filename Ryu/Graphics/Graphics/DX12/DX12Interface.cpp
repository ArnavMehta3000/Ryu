#include "DX12Interface.h"
#include "Graphics/GraphicsInterface.h"
#include "Graphics/DX12/DX12Core.h"

namespace Ryu::Graphics::DX12
{
	void GetGraphicsInterface(GraphicsInterface& gfx)
	{
		gfx.Init             = Core::Init;
		gfx.Shutdown         = Core::Shutdown;

		gfx.Surface.Create   = Core::CreateSurface;
		gfx.Surface.OnResize = Core::OnResizeSurface;
		gfx.Surface.Render   = Core::RenderSurface;
	}
}

#include "DX11Interface.h"
#include "Graphics/GraphicsInterface.h"
#include "Graphics/DX11/DX11Core.h"

namespace Ryu::Graphics::DX11
{
	void GetGraphicsInterface(GraphicsInterface& gfx)
	{
		gfx.Init     = Core::Init;
		gfx.Shutdown = Core::Shutdown;

		gfx.Surface.Create   = Core::CreateSurface;
		gfx.Surface.OnResize = Core::OnResizeSurface;
		gfx.Surface.Render   = Core::RenderSurface;
	}
}

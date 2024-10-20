#include "Renderer.h"
#include "Graphics/GraphicsAPI.h"
#include "Graphics/ISurface.h"
#include "Graphics/DX12/DX12Interface.h"
#include <libassert/assert.hpp>  //TODO: Remove this include when assert is removed from this file

namespace Ryu::Graphics
{
	namespace
	{
		GraphicsInterface g_gfx{};

		bool SetGraphicsInterface(API api)
		{
			switch (api)
			{
			case Ryu::Graphics::API::DirectX12:
				DX12::GetGraphicsInterface(g_gfx);
				return true;
			}

			return false;
		}
	}

	bool Init(API api)
	{
		return SetGraphicsInterface(api) && g_gfx.Init();
	}

	void Shutdown()
	{
		g_gfx.Shutdown();
	}

	ISurface* CreateSurface(App::WindowBase* window)
	{
		return g_gfx.Surface.Create(window);
	}

	void ResizeSurface(u32 width, u32 height)
	{
		g_gfx.Surface.OnResize(width, height);
	}

	void RenderSurface()
	{
		g_gfx.Surface.Render();
	}
}

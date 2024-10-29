#include "Renderer.h"
#include "Graphics/API.h"
#include "Graphics/GraphicsInterface.h"
#include "Graphics/ISurface.h"
#include "Graphics/Internal/DXInternal.h"
#include "Graphics/DX12/DX12Interface.h"
#include "Graphics/DX11/DX11Interface.h"

namespace Ryu::Graphics
{
	namespace
	{
		GraphicsInterface g_gfx{};
		bool g_initialized{ false };

		bool SetGraphicsInterface(API api)
		{
			switch (api)
			{
			case API::DirectX12: DX12::GetGraphicsInterface(g_gfx);
				return true;
			case API::DirectX11: DX11::GetGraphicsInterface(g_gfx);
				return true;
			}

			return false;
		}
	}

	bool IsInitialized()
	{
		return g_initialized;
	}

	bool Init(API api)
	{
		// Internal will create the common D3D objects
		g_initialized = Internal::Init(api)
			&& SetGraphicsInterface(api)
			&& g_gfx.Init();

		return g_initialized;
	}

	void Shutdown()
	{
		// Shutdown internal first since they are not relevant to the core rendering API
		// This will remove unwanted references
		Internal::Shutdown();
		g_gfx.Shutdown();
		g_initialized = false;
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

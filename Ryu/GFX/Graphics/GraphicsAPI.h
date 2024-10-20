#include "Common/Common.h"
#include "Graphics/ISurface.h"
#include "App/WindowBase.h"

namespace Ryu::Graphics
{
	// Contains function pointers for the graphics interface (API)
	struct GraphicsInterface
	{
		bool(*Init)(void);
		void(*Shutdown)(void);

		struct
		{
			ISurface*(*Create)(App::WindowBase* window);
			void(*OnResize)(u32 width, u32 height);
			void(*Render)();
		}Surface;
	};
}

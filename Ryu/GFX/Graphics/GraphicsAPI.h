#include "Common/Common.h"
#include "Graphics/Renderer.h"

namespace Ryu::Graphics
{
	// Contains function pointers for the graphics interface (API)
	struct GraphicsInterface
	{
		bool(*Init)(void);
		void(*Shutdown)(void);
	};
}

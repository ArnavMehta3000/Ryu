#pragma once
#include "Graphics/Core/GfxDevice.h"

namespace Ryu::Gfx
{
	class Renderer
	{
	public:
		Renderer(HWND window);

		void Render();
		void OnResize(u32 w, u32 h);

	private:
		GfxDevice m_device;
	};
}

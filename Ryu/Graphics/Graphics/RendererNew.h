#pragma once
#include "Graphics/Core/GfxDevice.h"

namespace Ryu::Gfx
{
	class RendererNew
	{
	public:
		RendererNew(HWND window);
		~RendererNew() {}

		void Render();
		void OnResize(u32 w, u32 h);

	private:
		std::unique_ptr<Device> m_device;
	};
}

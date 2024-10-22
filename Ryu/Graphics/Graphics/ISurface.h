#pragma once
#include "Common/Common.h"

namespace Ryu::App { class WindowBase; }

namespace Ryu::Graphics
{
	class ISurface
	{
	public:
		ISurface(App::WindowBase* window) : m_window(window) {}
		virtual ~ISurface() = default;

		virtual u32 GetWidth() const noexcept = 0;
		virtual u32 GetHeight() const noexcept = 0;
		
	protected:
		virtual void OnResize(u32 width, u32 height) = 0;
		virtual void Present() const = 0;

	protected:
		App::WindowBase* m_window{ nullptr };
	};

	struct GFXSurface
	{
		App::WindowBase* Window{ nullptr };
		ISurface* Surface{ nullptr };
	};

	// VVV Implemented in Renderer.cpp VVV

	ISurface* CreateSurface(App::WindowBase* window);
	void ResizeSurface(u32 width, u32 height);
	void RenderSurface();
}

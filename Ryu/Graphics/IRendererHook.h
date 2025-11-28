#pragma once

namespace Ryu::Gfx
{
	class Device;
	class CommandList;

	class IRendererHook
	{
#if defined(RYU_WITH_EDITOR)
	public:
		virtual void OnImGuiSetup(Gfx::Device* device) = 0;
		virtual void OnImGuiFrameBegin() = 0;
		virtual void OnImGuiFrameEnd(Gfx::CommandList* cmdList) = 0;
		virtual void OnImGuiRender() = 0;
		virtual void OnImGuiShutdown() = 0;
#endif
	};
}

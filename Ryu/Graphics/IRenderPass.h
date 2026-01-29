#pragma once
#include <nvrhi/nvrhi.h>

namespace Ryu::Gfx
{
	class RendererBase;

	class IRenderPass
	{
	public:
		explicit IRenderPass(RendererBase* renderer) : m_renderer(renderer) {}
		virtual ~IRenderPass() = default;

		[[nodiscard]] inline RendererBase* GetRenderer() const { return m_renderer; }

		virtual bool SupportsDepthBuffer() = 0;
		virtual void OnRender(nvrhi::IFramebuffer* framebuffer) = 0;
		virtual void OnBackBufferResizeBegin() = 0;
		virtual void OnBackBufferResizeEnd(const u32 width, const u32 height, const u32 sampleCount) = 0;

	private:
		RendererBase* m_renderer;;
	};
}

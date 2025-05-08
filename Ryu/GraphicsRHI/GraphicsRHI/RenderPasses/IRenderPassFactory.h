#pragma once
#include "GraphicsRHI/RenderPasses/IClearRenderPass.h"
#include <memory>

namespace Ryu::Graphics
{
	class IRenderPassFactory
	{
	public:
		virtual std::unique_ptr<IClearRenderPass> CreateClearRenderPass(const IClearRenderPass::Desc& desc) const = 0;
	};
}

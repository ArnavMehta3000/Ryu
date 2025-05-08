#pragma once
#include "GraphicsRHI/RenderPasses/IRenderPassFactory.h"

namespace Ryu::Graphics::DX11
{
	class DX11RenderPassFactory : public IRenderPassFactory
	{
	private:
		std::unique_ptr<IClearRenderPass> CreateClearRenderPass(const IClearRenderPass::Desc& desc) const override;
	};
}

#include "DX11RenderPassFactory.h"
#include "GraphicsDX11/RenderPasses/DX11ClearRenderPass.h"

namespace Ryu::Graphics::DX11
{
	std::unique_ptr<IClearRenderPass> DX11RenderPassFactory::CreateClearRenderPass(const IClearRenderPass::Desc& desc) const
	{
		return std::make_unique<DX11ClearRenderPass>(desc);
	}
}

#include "DX11ClearRenderPass.h"
#include "GraphicsDX11/DX11Device.h"
#include "GraphicsDX11/DX11CommandList.h"
#include "GraphicsDX11/DX11RenderTarget.h"

namespace Ryu::Graphics::DX11
{
	DX11ClearRenderPass::DX11ClearRenderPass(const Desc& desc)
		: IClearRenderPass(desc)
	{
	}

	void DX11ClearRenderPass::Begin(ICommandList* commandList)
	{
		DX11CommandList* dxCommandList = static_cast<DX11CommandList*>(commandList);
		
		RYU_TODO("Clear depth stencil");
		
		// Clear render targets
		for (IRenderTarget* rt : GetDesc().RenderTargets)
		{
			DX11RenderTarget* dxRT = static_cast<DX11RenderTarget*>(rt);
			dxCommandList->ClearRenderTargetView(dxRT, GetDesc().ClearColor.data());
		}
	}
	
	void DX11ClearRenderPass::End(MAYBE_UNUSED ICommandList* commandList)
	{
		// Do nothing
	}
}

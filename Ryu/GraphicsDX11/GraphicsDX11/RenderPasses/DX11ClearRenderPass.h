#pragma once
#include "Common/StandardTypes.h"
#include "GraphicsRHI/RenderPasses/IClearRenderPass.h"
#include <array>

namespace Ryu::Graphics::DX11
{
	class DX11ClearRenderPass : public IClearRenderPass
	{
	public:
		explicit DX11ClearRenderPass(const Desc& desc);

	private:
		void Begin(ICommandList* commandList) override;
		void End(ICommandList* commandList) override;
	};
}

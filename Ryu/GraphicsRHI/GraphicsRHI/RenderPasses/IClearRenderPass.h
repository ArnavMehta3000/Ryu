#pragma once
#include "Common/StandardTypes.h"
#include "GraphicsRHI/IRenderPass.h"
#include <vector>
#include <array>

namespace Ryu::Graphics
{
	class IClearRenderPass : public IRenderPass
	{
	public:
		struct Desc
		{
			std::vector<IRenderTarget*> RenderTargets;
			std::array<f32, 4> ClearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
		};

	public:
		virtual ~IClearRenderPass() = default;

	protected:
		IClearRenderPass(const Desc& desc) : m_desc(desc) {}
		const Desc& GetDesc() const noexcept { return m_desc; }

	private:
		Desc m_desc;
	};
}

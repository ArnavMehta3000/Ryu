#pragma once
#include "Graphics/Core/GfxDeviceChild.h"

namespace Ryu::Gfx
{
	class PipelineState : public DeviceChild
	{
	public:
		PipelineState(Device* parent, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, std::string_view name);
		virtual ~PipelineState() = default;

		virtual inline void ReleaseObject() override { ComRelease(m_pipelineState); }

		RYU_GFX_NATIVE(m_pipelineState)
	private:
		ComPtr<DX12::PipelineState> m_pipelineState;
	};
}

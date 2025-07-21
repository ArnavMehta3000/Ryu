#pragma once
#include "Graphics/Core/DeviceObject.h"

namespace Ryu::Gfx
{
	class PipelineState : public DeviceObject<PipelineState>
	{
		RYU_GFX_DEVICE_OBJ;
	public:
		RYU_GFX_NATIVE(m_pso)

		PipelineState() = default;
		PipelineState(DeviceWeakPtr parent, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
		~PipelineState();

	private:
		void OnConstruct(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
		void OnDestruct();

	private:
		ComPtr<DX12::PipelineState> m_pso;
	};
}

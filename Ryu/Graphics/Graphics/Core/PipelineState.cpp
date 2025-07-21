#include "Graphics/Core/PipelineState.h"
#include "Graphics/Core/Device.h"

namespace Ryu::Gfx
{
	PipelineState::PipelineState(DeviceWeakPtr parent, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
		:DeviceObject(parent)
	{
		OnConstruct(desc);
	}

	PipelineState::~PipelineState()
	{
		OnDestruct();
	}

	void PipelineState::OnConstruct(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
	{
		if (auto parent = GetParent())
		{
			DX12::Device* const device = parent->GetDevice();
			DXCallEx(device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&m_pso)), device);
			SetName("Pipeline State");
		}
	}

	void PipelineState::OnDestruct()
	{
		if (m_pso)
		{
			m_pso.Reset();
		}
	}
}

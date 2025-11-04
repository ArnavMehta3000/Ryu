#include "Graphics/Core/GfxPipelineState.h"
#include "Graphics/Core/GfxDevice.h"

namespace Ryu::Gfx
{
	PipelineState::PipelineState(Device* parent, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, std::string_view name)
		: DeviceChild(parent)
	{
		DX12::Device* device = GetDevice()->GetNativeDevice();

		DXCall(device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&m_pipelineState)));
		DX12::SetObjectName(m_pipelineState.Get(), name.data());
	}
}

#include "Graphics/Core/CommandContext.h"
#include "Graphics/Core/Device.h"
#include "Profiling/Profiling.h"

namespace Ryu::Gfx
{
	CommandContext::CommandContext(std::weak_ptr<Device> parent, CommandListType type)
		: DeviceObject(parent)
		, m_type(type)
		, m_frameIndex(0)
	{
		OnConstruct(type);
	}

	CommandContext::~CommandContext()
	{
		OnDestruct();
	}

	void CommandContext::OnConstruct(CommandListType type)
	{
		RYU_PROFILE_SCOPE();

		if (auto parent = GetParent())
		{
			const D3D12_COMMAND_LIST_TYPE nativeType = DX12::ToNative(type);
			DX12::Device* const device = parent->GetDevice();
			DXCallEx(device->CreateCommandAllocator(nativeType, IID_PPV_ARGS(&m_commandAllocator)), parent->GetDevice());
		}
	}
}

#include "Graphics/Core/CommandAllocator.h"
#include "Graphics/Core/Device.h"

namespace Ryu::Gfx
{
	CommandAllocator::CommandAllocator(DeviceWeakPtr parent, CommandListType type)
		: DeviceObject(parent)
	{
		OnConstruct(type);
	}
	
	CommandAllocator::~CommandAllocator()
	{
		OnDestruct();
	}

	void CommandAllocator::Reset() const
	{
		DXCall(m_allocator->Reset());
	}

	void CommandAllocator::OnConstruct(CommandListType type)
	{
		if (auto parent = GetParent())
		{
			DX12::Device* const device = parent->GetDevice();

			D3D12_COMMAND_LIST_TYPE nativeType = DX12::ToNative(type);
			DXCallEx(device->CreateCommandAllocator(nativeType, IID_PPV_ARGS(&m_allocator)), device);
		}
	}

	void CommandAllocator::OnDestruct()
	{
		if (m_allocator)
		{
			m_allocator.Reset();
		}
	}
}

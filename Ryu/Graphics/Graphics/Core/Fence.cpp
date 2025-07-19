#include "Graphics/Core/Fence.h"
#include "Graphics/Core/Device.h"

namespace Ryu::Gfx
{
	Fence::Fence(DeviceWeakPtr parent, u64 initialValue, FenceFlag flag)
		: DeviceObject(parent)
	{
		OnConstruct(initialValue, flag);
	}

	Fence::~Fence()
	{
		OnDestruct();
	}

	void Fence::OnConstruct(u64 initialValue, FenceFlag flag)
	{
		if (auto parent = GetParent())
		{
			DX12::Device* const device = parent->GetDevice();
			DXCallEx(device->CreateFence(initialValue, DX12::ToNative(flag), IID_PPV_ARGS(&m_fence)), device);
			SetName("Frame Fence");
		}
	}

	void Fence::OnDestruct()
	{
		if (m_fence)
		{
			m_fence.Reset();
		}
	}
}

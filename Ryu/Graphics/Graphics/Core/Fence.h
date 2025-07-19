#pragma once
#include "Graphics/Core/DeviceObject.h"

namespace Ryu::Gfx
{
	class Fence : public DeviceObject<Fence>
	{
		RYU_GFX_DEVICE_OBJ;
	public:
		RYU_GFX_NATIVE(m_fence)

		Fence() = default;
		Fence(DeviceWeakPtr parent, u64 initialValue, FenceFlag flag );
		~Fence();

	private:
		void OnConstruct(u64 initialValue, FenceFlag flag );
		void OnDestruct();

	private:
		ComPtr<DX12::Fence> m_fence;
	};
}

#pragma once
#include "Graphics/Core/DeviceObject.h"

namespace Ryu::Gfx
{
	class CommandQueue : public DeviceObject<CommandQueue>
	{
		RYU_GFX_DEVICE_OBJ;
	public:
		RYU_GFX_NATIVE(m_queue)

		CommandQueue() = default;
		CommandQueue(DeviceWeakPtr parent, CommandListType type, CommandQueuePriority priority);

		void Signal(Fence& fence, u64 value) const;

	private:
		void OnConstruct(CommandListType type, CommandQueuePriority priority);
		void OnDestruct();

	private:
		ComPtr<DX12::CommandQueue> m_queue;
	};
}

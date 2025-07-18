#pragma once
#include "Graphics/Core/DeviceObject.h"

namespace Ryu::Gfx
{
	class CommandContext
	{
		RYU_GFX_DEVICE_OBJ;
	public:
		CommandContext() = default;
		CommandContext(std::weak_ptr<Device> parent);
		~CommandContext();

		void Begin();
		void End();

	private:
		ComPtr<DX12::GraphicsCommandList> m_commandList;
		ComPtr<DX12::CommandAllocator> m_commandAllocator;
		CommandListType m_type;
		u32 m_frameIndex;
		std::vector<CD3DX12_RESOURCE_BARRIER> m_pendingBarriers;
	};
}

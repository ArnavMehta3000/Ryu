#pragma once
#include "Graphics/Core/DeviceObject.h"
#include <span>

namespace Ryu::Gfx
{
	class CommandContext
	{
	public:
		explicit CommandContext(CommandList* cmdList, CommandAllocator* allocator);
		~CommandContext();
		
		void Begin();
		void End();

		void SetResourceBarrier(const CD3DX12_RESOURCE_BARRIER& barrier);
		void SetResourceBarriers(std::span<const CD3DX12_RESOURCE_BARRIER> barriers);

	private:
		CommandList* m_cmdList{ nullptr };
		CommandAllocator* m_allocator{ nullptr };
	};
}

#pragma once
#include "Graphics/Core/DeviceObject.h"

namespace Ryu::Gfx
{
	class CommandContext
	{
	public:
		CommandContext() = default;
		explicit CommandContext(CommandList* cmdList, CommandAllocator* allocator);
		~CommandContext();
		
		void Begin();
		void End();

	private:
		CommandList* m_cmdList{ nullptr };
		CommandAllocator* m_allocator{ nullptr };
	};
}

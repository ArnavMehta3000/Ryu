#include "Graphics/Core/CommandContext.h"
#include "Graphics/Core/CommandAllocator.h"
#include "Graphics/Core/CommandList.h"
#include "Profiling/Profiling.h"

namespace Ryu::Gfx
{
	CommandContext::CommandContext(CommandList* cmdList, CommandAllocator* allocator)
		: m_cmdList(cmdList)
		, m_allocator(allocator)
	{
		Begin();
	}

	CommandContext::~CommandContext()
	{
		End();
	}

	void CommandContext::Begin()
	{
		m_allocator->Reset();
		m_cmdList->Reset(*m_allocator);  // TODO: PSO
	}

	void CommandContext::End()
	{
		m_cmdList->Close();
	}
}

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
		m_cmdList->Reset(*m_allocator);  RYU_TODO("Implement PSO");
	}

	void CommandContext::End()
	{
		m_cmdList->Close();
	}
	
	void CommandContext::SetResourceBarrier(const CD3DX12_RESOURCE_BARRIER& barrier)
	{
		m_cmdList->Get()->ResourceBarrier(1, &barrier);
	}
	
	void CommandContext::SetResourceBarriers(std::span<const CD3DX12_RESOURCE_BARRIER> barriers)
	{
		m_cmdList->Get()->ResourceBarrier(static_cast<u32>(barriers.size()), barriers.data());
	}
}

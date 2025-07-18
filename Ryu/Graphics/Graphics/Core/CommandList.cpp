#include "Graphics/Core/CommandList.h"
#include "Graphics/Core/CommandAllocator.h"

namespace Ryu::Gfx
{
	CommandList::CommandList(ComPtr<DX12::GraphicsCommandList> commandList)
		: m_cmdList(std::move(commandList))
	{
	}
	
	void CommandList::Reset(CommandAllocator& allocator, PipelineState* pso) const
	{
		RYU_TODO("Implement PSO");
		DXCall(m_cmdList->Reset(allocator, nullptr));
	}

	void CommandList::Close() const
	{
		DXCall(m_cmdList->Close());
	}
}

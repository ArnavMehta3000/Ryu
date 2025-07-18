#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	class CommandList
	{
	public:
		RYU_GFX_NATIVE(m_cmdList)

		CommandList() = default;
		explicit CommandList(ComPtr<DX12::GraphicsCommandList> commandList);

		inline void SetName(const char* name) const { DX12::SetObjectName(m_cmdList.Get(), name); }

		inline void Reset(CommandAllocator& allocator, PipelineState* pso = nullptr) const;
		inline void Close() const;

	private:
		ComPtr<DX12::GraphicsCommandList> m_cmdList;
	};
}

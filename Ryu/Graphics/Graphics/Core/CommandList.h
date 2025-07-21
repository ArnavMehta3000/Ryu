#pragma once
#include "Graphics/Core/DeviceObject.h"

namespace Ryu::Gfx
{
	class CommandList : public DeviceObject<CommandList>
	{
		RYU_GFX_DEVICE_OBJ;
	public:
		enum class State
		{
			Closed,
			Recording
		};

	public:
		RYU_GFX_NATIVE(m_cmdList)

		CommandList() = default;
		explicit CommandList(DeviceWeakPtr parent, CommandAllocator& allocator, PipelineState* pso, CommandListType type);
		~CommandList();

		inline State GetState() const { return m_state; }

		inline void Reset(CommandAllocator& allocator, PipelineState* pso = nullptr) const;
		inline void Close() const;

	private:
		void OnConstruct(CommandAllocator& allocator, PipelineState* pso, CommandListType type);
		void OnDestruct();

	private:
		mutable State m_state = State::Closed;
		ComPtr<DX12::GraphicsCommandList> m_cmdList;
	};
}

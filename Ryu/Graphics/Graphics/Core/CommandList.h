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
		explicit CommandList(DeviceWeakPtr parent, CommandAllocator& allocator, CommandListType type);
		~CommandList();

		inline void Reset(CommandAllocator& allocator, PipelineState* pso = nullptr) const;
		inline void Close() const;

	private:
		void OnConstruct(CommandAllocator& allocator, CommandListType type);
		void OnDestruct();

	private:
		mutable State m_state;
		ComPtr<DX12::GraphicsCommandList> m_cmdList;
	};
}

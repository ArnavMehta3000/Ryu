#pragma once
#include "Graphics/Core/DeviceObject.h"

namespace Ryu::Gfx
{
	class CommandList : public DeviceObject<CommandList>
	{
		RYU_GFX_DEVICE_OBJ;
	public:
		CommandList() = default;
		CommandList(std::weak_ptr<Device> parent, CommandListType type);
		~CommandList();

		void ResetCommandList(DX12::CommandAllocator* allocator = nullptr);
		void CloseCommandList();

		inline NODISCARD DX12::GraphicsCommandList* const GetCommandList() const { return m_cmdList.Get(); }
		inline NODISCARD DX12::CommandAllocator* const GetCommandAllocator() const { return m_cmdAllocator.Get(); }
		inline NODISCARD bool IsInFlight() const { return m_inFlight; }

	private:
		void OnConstruct(CommandListType type);

	private:
		ComPtr<DX12::GraphicsCommandList> m_cmdList;
		ComPtr<DX12::CommandAllocator> m_cmdAllocator;
		bool m_inFlight{ false };
	};
}

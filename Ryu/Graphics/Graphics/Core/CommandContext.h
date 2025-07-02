#pragma  once
#include "Graphics/Core/DeviceObject.h"
#include "Graphics/Core/Fence.h"
#include <span>

namespace Ryu::Gfx
{
	class CommandCtx : public DeviceObject<CommandCtx>
	{
		RYU_GFX_DEVICE_OBJ;
	public:
		CommandCtx() = default;
		CommandCtx(std::weak_ptr<Device> parent, CommandListType type, CommandQueuePriority priority = CommandQueuePriority::Normal);
		~CommandCtx() = default;

		void ExecuteCommandList(CommandList* cmdList);
		void ExecuteCommandLists(std::span<CommandList*> cmdLists);
		void Signal(Fence& fence, u64 value);
		void Wait(Fence& fence, u64 value);
		void Flush(Fence& fence);

		inline NODISCARD DX12::CommandQueue* const GetCommandQueue() const noexcept{ return m_cmdQueue.Get(); }
		inline NODISCARD CommandListType GetType() const noexcept { return m_type; }

	private:
		void OnConstruct(CommandListType type, CommandQueuePriority priority = CommandQueuePriority::Normal);

	private:
		CommandListType            m_type;
		ComPtr<DX12::CommandQueue> m_cmdQueue;
	};
}

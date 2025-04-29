#pragma once
#include "Graphics/DeviceResource.h"
#include "Graphics/Fence.h"
#include <span>

namespace Ryu::Gfx
{
	class CmdQueue : public DeviceObject
	{
	public:
		CmdQueue(Device* parent, D3D12_COMMAND_LIST_TYPE type);

		SyncPoint ExecuteCommandLists(std::span<CommandContext* const> contexts);

		void InsertWait(const SyncPoint& syncPoint);
		void InsertWait(CmdQueue* cmdQueue);

		void CPUWaitForIdle();
		ComPtr<DX12::CmdAllocator> RequestAllocator();
		void FreeCmdAllocator(const SyncPoint& syncPoint, ComPtr<DX12::CmdAllocator>& allocator);

		inline NODISCARD DX12::CmdQueue* GetCmdQueue() const { return m_cmdQueue.Get(); }
		inline NODISCARD Fence* GetFence() const { return m_fence.Get(); }
		inline NODISCARD u64 GetTimestampFrequency() const { return m_timestampFrequency; }

	private:
		ComPtr<DX12::CmdQueue>     m_cmdQueue;
		RYU_TODO("Make fenced pool?");
		Memory::Ref<Fence>         m_fence;
		SyncPoint                  m_syncPoint;
		D3D12_COMMAND_LIST_TYPE    m_type;
		u64                        m_timestampFrequency;
		std::mutex                 m_executeMutex;
	};
}

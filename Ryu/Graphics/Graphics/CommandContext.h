#pragma once
#include "Graphics/DeviceResource.h"

namespace Ryu::Gfx
{
	class CommandContext : public DeviceObject
	{
		RYU_DISABLE_COPY_AND_MOVE(CommandContext)
	public:
		CommandContext(Device* parent, CmdListType type);
		~CommandContext();

		void BeginFrame();
		void EndFrame();
		void Flush();

		inline NODISCARD DX12::CmdQueue* const GetCmdQueue() const noexcept { return m_cmdQueue.Get(); }
		inline NODISCARD DX12::GfxCmdList* const GetGfxCmdList() const noexcept { return m_cmdList.Get(); }
		inline NODISCARD u32 GetFrameIndex() const noexcept { return m_frameIndex; }

	private:
		struct CommandFrame
		{
			ComPtr<DX12::CmdAllocator> CmdAllocator;
			u64 FenceValue = 0;

			void ReleaseAllocator()
			{
				CmdAllocator.Reset();
				FenceValue = 0;
			}
			
			void Wait(HANDLE fenceEvent, DX12::Fence* fence);
		};

		ComPtr<DX12::CmdQueue>   m_cmdQueue;
		ComPtr<DX12::GfxCmdList> m_cmdList;
		ComPtr<DX12::Fence>      m_fence;
		CommandFrame             m_cmdFrames[FRAME_BUFFER_COUNT];
		u32                      m_frameIndex;
		HANDLE                   m_fenceEvent;
		u64                      m_fenceValue;
	};
}

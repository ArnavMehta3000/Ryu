#pragma once
#include "Graphics/DeviceResource.h"

namespace Ryu::Gfx
{
	class CommandContext : public DeviceObject
	{
	public:
		CommandContext(Device* parent, CmdListType type);
		~CommandContext();

		void BeginFrame();
		void EndFrame();

	private:
		struct CommandFrame
		{
			ComPtr<DX12::CmdAllocator> CmdAllocator;

			void Release()
			{
				CmdAllocator.Reset();
			}
			
			void Wait();
		};

		ComPtr<DX12::CmdQueue> m_cmdQueue;
		ComPtr<DX12::GfxCmdList> m_cmdList;
		CommandFrame m_cmdFrames[FRAME_BUFFER_COUNT];
		u32 m_frameIndex;
	};
}

#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	class CommandContext2
	{
		RYU_DISABLE_COPY_AND_MOVE(CommandContext2);

		struct CommandFrame
		{
			ComPtr<DX12::CommandAllocator> Allocator;
			u64 FenceValue;

			void Wait(HANDLE fenceEvent, const ComPtr<DX12::Fence>& fence);
			void Release();
		};

	public:
		CommandContext2() = default;
		~CommandContext2();

		void Create(CommandListType type);
		void Destroy();

		void BeginFrame();
		void EndFrame();
		void Flush();

		inline const auto& GetCommandQueue() const { return m_cmdQueue; }
		inline const auto& GetCommandList() const { return m_cmdList; }
		inline u32 GetCurrentFrameIndex() const { return m_frameIndex; }

	private:
		void CreateCommandQueue(DX12::Device* device, CommandListType type);
		void CreateCommandAllocators(DX12::Device* device, CommandListType type);
		void CreateCommandList(DX12::Device* device, CommandListType type);
		void CreateSynchronization(DX12::Device* device);

	private:
		u64                                m_fenceValue = 0;
		HANDLE                             m_fenceEvent = nullptr;
		u32                                m_frameIndex = 0;
		ComPtr<DX12::Fence>                m_fence      = nullptr;
		ComPtr<DX12::GraphicsCommandList>  m_cmdList    = nullptr;
		ComPtr<DX12::CommandQueue>         m_cmdQueue   = nullptr;
		FrameArray<CommandFrame>           m_cmdFrames;
	};
}

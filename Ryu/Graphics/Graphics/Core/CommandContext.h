#pragma once
#include "Graphics/Core/DeviceObject.h"
#include "Graphics/Core/CommandList.h"
#include "Graphics/Core/CommandQueue.h"
#include "Graphics/Core/CommandAllocator.h"
#include "Graphics/Core/Fence.h"
#include <span>

namespace Ryu::Gfx
{
	class CommandContext
	{
	public:
		explicit CommandContext(CommandList* cmdList, CommandAllocator* allocator, PipelineState* pso);
		~CommandContext();
		
		void Begin(PipelineState* pso);
		void End();

		void SetResourceBarrier(const CD3DX12_RESOURCE_BARRIER& barrier);
		void SetResourceBarriers(std::span<const CD3DX12_RESOURCE_BARRIER> barriers);

	private:
		CommandList* m_cmdList{ nullptr };
		CommandAllocator* m_allocator{ nullptr };
	};


	class Command : public DeviceObject<Command>
	{
		RYU_DISABLE_COPY_AND_MOVE(Command)
		RYU_GFX_DEVICE_OBJ;
	public:
		Command() {}
		Command(DeviceWeakPtr parent, CommandListType type);
		~Command();

		void BeginFrame(PipelineState* pso);
		void EndFrame();
		void Flush();

		void SetResourceBarrier(const CD3DX12_RESOURCE_BARRIER& barrier);
		void SetResourceBarriers(std::span<const CD3DX12_RESOURCE_BARRIER> barriers);

		inline CommandList& GetCommandList() { return m_cmdList; }
		inline CommandQueue& GetCommandQueue() { return m_cmdQueue; }
		inline u32 GetFrameIndex() const { return m_frameIndex; }

	private:
		void OnConstruct(CommandListType type);
		void OnDestruct();

	private:
		struct CommandFrame
		{
			CommandAllocator Allocator;
			u64 FenceValue = 0;

			void Wait(HANDLE fenceEvent, Fence& fence);
			void Release();
		};

	private:
		u64                      m_fenceValue{ 0 };
		HANDLE                   m_fenceEvent{ nullptr };
		u32                      m_frameIndex{ 0 };
		Fence                    m_fence;
		CommandList              m_cmdList;
		CommandQueue             m_cmdQueue;
		FrameArray<CommandFrame> m_cmdFrames;
	};
}

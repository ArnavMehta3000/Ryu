#pragma once
#include "Graphics/Core/DX12.h"
#include <functional>

namespace Ryu::Gfx
{
	class GfxDevice;

	struct FrameSyncData
	{
		ComPtr<DX12::CommandAllocator> Allocator;
		u64 FenceValue = 0;
	};

	class CommandContext
	{
	public:
		CommandContext(GfxDevice* device, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		~CommandContext();

		RYU_DISABLE_COPY(CommandContext)

		[[nodiscard]] inline auto GetQueue() const { return m_cmdQueue.Get(); }
		[[nodiscard]] inline auto GetCommandList() const { return m_cmdList.Get(); }
		[[nodiscard]] inline u32 GetCurrentFrameIndex() const { return m_frameIndex; }

		void BeginFrame(u32 frameIndex, DX12::PipelineState* pipelineState);
		void EndFrame();
		void ExecuteCommandList();

		void WaitForGPU();
		void MoveToNextFrame(u32 nextFrameIndex);
		u64 SignalFence();
		void WaitForFenceValue(u64 fenceValue);

		void ExecuteImmediate(std::function<void(ID3D12GraphicsCommandList*)> recordFunc);

	private:
		void CreateCommandQueue();
		void CreateCommandAllocators();
		void CreateCommandList();
		void CreateSynchronizationObjects();

	private:
		GfxDevice*                            m_device = nullptr;
		D3D12_COMMAND_LIST_TYPE               m_type;

		ComPtr<DX12::CommandQueue>            m_cmdQueue;
		ComPtr<DX12::GraphicsCommandList>     m_cmdList;
		ComFrameArray<DX12::CommandAllocator> m_cmdAllocators;

		ComPtr<DX12::Fence>                   m_fence;
		HANDLE                                m_fenceEvent = nullptr;
		FrameArray<u64>                       m_fenceValues{};
		u32                                   m_frameIndex = 0;
	};
}

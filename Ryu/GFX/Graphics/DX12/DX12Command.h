#pragma once
#include "Graphics/Common.h"
#include <array>

namespace Ryu::Graphics::DX12::Core
{
	class DX12Command
	{
	public:
		RYU_DISABLE_COPY_AND_MOVE(DX12Command);

		DX12Command() = default;
		explicit DX12Command(ID3D12Device8* const device, D3D12_COMMAND_LIST_TYPE type);
		~DX12Command();
		void Flush();
		void Release();
		void BeginFrame();
		void EndFrame();

		constexpr inline ID3D12GraphicsCommandList6* const GetCommandList() const { return m_cmdList; }
		constexpr inline ID3D12CommandQueue* const GetCommandQueue() const { return m_cmdQueue; }
		constexpr inline u64 GetFenceValue() const { return m_fenceValue; }
		constexpr inline u32 GetFrameIndex() const { return m_frameIndex; }

	private:
		struct CommandFrame
		{
			ID3D12CommandAllocator* Allocator{ nullptr };
			u64 FenceValue{ 0 };

			void Wait(HANDLE fenceEvent, ID3D12Fence1* fence) const noexcept;
			void Release() noexcept { Graphics::Release(Allocator); FenceValue = 0; }
		};

	private:
		ID3D12CommandQueue*                          m_cmdQueue{ nullptr };
		ID3D12GraphicsCommandList6*                  m_cmdList{ nullptr };
		std::array<CommandFrame, FRAME_BUFFER_COUNT> m_cmdFrames{};
		u32                                          m_frameIndex{ 0 };
		u64                                          m_fenceValue{ 0 };
		ID3D12Fence1*                                m_fence{ nullptr };
		HANDLE                                       m_fenceEvent{ nullptr };
	};
}

#pragma once
#include "Graphics/Core/DeviceObject.h"

namespace Ryu::Gfx
{
	class CommandContext : public DeviceObject<CommandContext>
	{
		RYU_GFX_DEVICE_OBJ;
	public:
		CommandContext() = default;
		CommandContext(std::weak_ptr<Device> parent, CommandListType type = CommandListType::Direct);
		~CommandContext();

		void Begin();
		void End();
		void Execute();
		void Reset();

		void TransitionResource(DX12::Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
		void FlushResourceBarriers();

		void SetRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE& rtv, const D3D12_CPU_DESCRIPTOR_HANDLE* dsv = nullptr);
		void ClearRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE& rtv, const float* clearColor);
		void ClearDepthStencil(const D3D12_CPU_DESCRIPTOR_HANDLE& dsv, float depth = 1.0f, u8 stencil = 0);

		void SetViewport(const D3D12_VIEWPORT& viewport);
		void SetScissorRect(const D3D12_RECT& rect);

		inline DX12::GraphicsCommandList* GetCommandList() const { return m_commandList.Get(); }
		inline u32 GetFrameIndex() const { return m_frameIndex; }

	private:
		void OnConstruct(CommandListType type = CommandListType::Direct);
		void OnDestruct();

	private:
		ComPtr<DX12::GraphicsCommandList> m_commandList;
		ComPtr<DX12::CommandAllocator> m_commandAllocator;
		CommandListType m_type;
		u32 m_frameIndex;
		std::vector<CD3DX12_RESOURCE_BARRIER> m_pendingBarriers;
	};
}

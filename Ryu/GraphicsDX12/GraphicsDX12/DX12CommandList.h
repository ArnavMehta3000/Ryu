#pragma once
#include "GraphicsDX12/DX12Types.h"
#include "GraphicsRHI/ICommandList.h"
#include "GraphicsDX12/DX12DeviceResource.h"
#include "Logger/LogCategory.h"

namespace Ryu::Graphics::DX12
{
	class DX12Device;

	class DX12CommandList 
		: public DX12DeviceResource
		, public ICommandList
		, public IGraphicsRHIObject<DX12::IDX12GraphicsCommandList>
	{
		RYU_LOG_CATEGORY(DX12CommandList);

	public:
		RYU_DECLARE_GFX_NATIVE_TYPE_OP(m_gfxCmdList.Get())

		DX12CommandList(const DX12Device* device, const CommandListDesc& desc);
		~DX12CommandList();

		inline DX12::IDX12CommandAllocator* GetCommandAllocator() const noexcept { return m_cmdAllocator.Get(); }
		inline DX12::IDX12Fence* GetFence() const noexcept { return m_fence.Get(); }
		inline HANDLE GetFenceEvent() const noexcept { return m_fenceEvent; }
		inline u64 GetFenceValue() const noexcept { return m_fenceValue; }

		void WaitForGPU(DWORD milliseconds = INFINITE) const;
		u64 Signal() const;
		void Flush();

		void ClearRenderTargetView(IRenderTarget* renderTarget, const f32* clearColor) override;

	private:
		void Begin() override;
		void End() override;
		void Reset() override;
		
		void Initialize();

	private:
		ComPtr<NativeType>                  m_gfxCmdList;
		ComPtr<DX12::IDX12CommandAllocator> m_cmdAllocator;
		ComPtr<DX12::IDX12Fence>            m_fence;
		HANDLE                              m_fenceEvent;
		mutable u64                         m_fenceValue;
	};
}

#pragma once
#include "Common/StandardTypes.h"
#include "Graphics/DX12/DX12Types.h"
#include "Graphics/DX12/IDX12Object.h"
#include "Graphics/Shared/Interface/ICommandList.h"

namespace Ryu::Graphics
{
	class DX12Device;

	class DX12CommandList : public ICommandList, public IDX12Object
	{
	public:
		RYU_DECLARE_GFX_NATIVE_TYPE(DX12::IDX12GraphicsCommandList);
		RYU_DECLARE_GFX_NATIVE_TYPE_OP(m_gfxCmdList.Get())

		DX12CommandList(const DX12Device& device, const CommandListDesc& desc);
		~DX12CommandList();

		inline DX12::IDX12CommandAllocator* GetCommandAllocator() const { return m_cmdAllocator.Get(); }
		inline DX12::IDX12Fence* GetFence() const { return m_fence.Get(); }
		inline HANDLE GetFenceEvent() const { return m_fenceEvent; }
		inline u64 GetFenceValue() const { return m_fenceValue; }

		void WaitForGPU() const;
		void Signal() const;

	private:
		void Begin() override;
		void End() override;
		void Reset() override;
		
		void Initialize();

	private:
		const DX12Device&                   m_device;
		ComPtr<NativeType>                  m_gfxCmdList;
		ComPtr<DX12::IDX12CommandAllocator> m_cmdAllocator;
		ComPtr<DX12::IDX12Fence>            m_fence;
		HANDLE                              m_fenceEvent;
		mutable u64                         m_fenceValue;
	};
}

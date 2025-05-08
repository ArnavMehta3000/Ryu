#pragma once
#include "GraphicsDX12/DX12Types.h"
#include "GraphicsRHI/IDevice.h"

namespace Ryu::Graphics::DX12
{
	class DX12Device final : public IDevice, public IGraphicsRHIObject<DX12::IDX12Device>
	{
	public:
		RYU_DECLARE_GFX_NATIVE_TYPE_OP(m_device.Get())

		DX12Device();
		virtual ~DX12Device();

		inline DX12::IDX12CommandQueue* GetCommandQueue() const { return m_cmdQueue.Get(); }

		DX12::IDX12Fence* CreateFence(u64 initialValue = 0, D3D12_FENCE_FLAGS flags = D3D12_FENCE_FLAG_NONE) const;
		DX12::IDX12DescriptorHeap* CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, u32 count);

	private:
		void ReportLiveObjects(bool releaseBeforeReporting = false) override;
		CreateSwapChainResult CreateSwapChain(const SwapChainDesc& desc) const override;
		CreateCommandListResult CreateCommandList(const CommandListDesc& desc) const override;
		void ExecuteCommandList(const ICommandList* commandList) const override;

		void InitDevice();
		void CreateCommandQueue();
		void CreateDescriptorHeaps();

	private:
		ComPtr<NativeType>                m_device;
		ComPtr<DX12::IDX12CommandQueue>   m_cmdQueue;
	};
}

#pragma once
#include "Graphics/DX12/DX12Types.h"
#include "Graphics/Shared/Interface/IDevice.h"

namespace Ryu::Graphics
{
	class DX12Device : public IDevice
	{
	public:
		RYU_DECLARE_GFX_NATIVE_TYPE(DX12::IDX12Device);
		RYU_DECLARE_GFX_NATIVE_TYPE_OP(m_device.Get())

		DX12Device(const DeviceCreateDesc & desc);
		~DX12Device();

		inline DX12::IDX12CommandQueue* GetCommandQueue() const { return m_cmdQueue.Get(); }

	private:
		void ReportLiveObjects(bool releaseBeforeReporting = false) override;
		NativeObjectType GetNativeObject() const override;
		CreateSwapChainResult CreateSwapChain(const SwapChainDesc& desc) const override;
		CreateCommandListResult CreateCommandList(const CommandListDesc& desc) const override;
		void ExecuteCommandList(const ICommandList* commandList) const override;

		void InitDevice(const DeviceCreateDesc& desc);

	private:
		ComPtr<NativeType> m_device;
		ComPtr<DX12::IDX12CommandQueue> m_cmdQueue;
	};
}

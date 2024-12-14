#pragma once
#include "Graphics/DX12/DX12Types.h"
#include "Graphics/Shared/Interface/IDevice.h"

namespace Ryu::Graphics
{
	class DX12Device : public IDevice
	{
	public:
		RYU_DECLARE_GFX_NATIVE_TYPE(DX12::IDX12Device);

		DX12Device(const DeviceCreateDesc & desc);
		~DX12Device();

	private:
		void ReportLiveObjects(bool releaseBeforeReporting = false) override;
		NativeObjectType GetNativeObject() const override;
		CreateSwapChainResult CreateSwapChain(const SwapChainDesc& desc) override;

		inline DX12::IDX12CommandQueue* GetCommandQueue() const { return m_cmdQueue.Get(); }

		void InitDevice(const DeviceCreateDesc& desc);

	private:
		ComPtr<DX12::IDX12Device> m_device;
		ComPtr<DX12::IDX12CommandQueue> m_cmdQueue;
	};
}

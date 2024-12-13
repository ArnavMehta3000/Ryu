#pragma once
#include "Graphics/DX12/DX12Types.h"
#include "Graphics/Shared/Interface/IDevice.h"

namespace Ryu::Graphics
{
	class DX12Device : public IDevice
	{
	public:
		DX12Device(const DeviceCreateDesc & desc);

	private:
		NativeObjectType GetNativeObject() const override;
		CreateSwapChainResult CreateSwapChain(const SwapChainDesc& desc) override;

		inline DX12::IDX12CommandQueue* GetCommandQueue() const { return m_cmdQueue.Get(); }

	private:
		ComPtr<DX12::IDX12Device> m_device;
		ComPtr<DX12::IDX12CommandQueue> m_cmdQueue;
	};
}

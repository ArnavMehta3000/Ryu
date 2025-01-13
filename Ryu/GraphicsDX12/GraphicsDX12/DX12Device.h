#pragma once
#include "GraphicsDX12/DX12Types.h"
#include "GraphicsRHI/IDevice.h"

namespace Ryu::Graphics::DX12
{
	class DX12Device : public IDevice, public IGraphicsRHIObject<DX12::IDX12Device>
	{
	public:
		RYU_DECLARE_GFX_NATIVE_TYPE_OP(m_device.Get())

		DX12Device(const DeviceCreateDesc& desc);
		~DX12Device();

		inline DX12::IDX12CommandQueue* GetCommandQueue() const { return m_cmdQueue.Get(); }
		inline ID3D12DescriptorHeap* GetRTVHeap() const { return m_rtvHeap.Get(); }
		inline ID3D12DescriptorHeap* GetDSVHeap() const { return m_dsvHeap.Get(); }
		inline u32 GetRTVDescriptorSize() const { return m_rtvHeapSize; }
		inline u32 GetDSVDescriptorSize() const { return m_dsvHeapSize; }

	private:
		void ReportLiveObjects(bool releaseBeforeReporting = false) override;
		CreateSwapChainResult CreateSwapChain(const SwapChainDesc& desc) const override;
		CreateCommandListResult CreateCommandList(const CommandListDesc& desc) const override;
		void ExecuteCommandList(const ICommandList* commandList) const override;

		void InitDevice(const DeviceCreateDesc& desc);
		void CreateCommandQueue();
		void CreateDescriptorHeaps();

	private:
		ComPtr<NativeType>                m_device;
		ComPtr<DX12::IDX12CommandQueue>   m_cmdQueue;
		ComPtr<DX12::IDX12DescriptorHeap> m_rtvHeap;
		ComPtr<DX12::IDX12DescriptorHeap> m_dsvHeap;
		u32                               m_rtvHeapSize;
		u32                               m_dsvHeapSize;
	};
}

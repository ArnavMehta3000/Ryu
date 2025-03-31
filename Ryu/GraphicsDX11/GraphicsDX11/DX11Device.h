#pragma once
#include "GraphicsDX11/DX11Types.h"
#include "GraphicsRHI/IDevice.h"

namespace Ryu::Graphics::DX11
{
	class DX11Device : public IDevice, public IGraphicsRHIObject<DX11::IDX11Device>
	{
	public:
		RYU_DECLARE_GFX_NATIVE_TYPE_OP(m_device.Get())

		DX11Device();
		~DX11Device();

		inline DX11::IDX11DeviceContext* GetImmediateContext() const { return m_imContext.Get(); }

	private:
		void ReportLiveObjects(bool releaseBeforeReporting = false) override;
		CreateSwapChainResult CreateSwapChain(const SwapChainDesc& desc) const override;
		CreateCommandListResult CreateCommandList(const CommandListDesc& desc) const override;
		void ExecuteCommandList(const ICommandList* commandList) const override;

		void InitDevice();
	
	private:
		ComPtr<NativeType>               m_device;
		ComPtr<DX11::IDX11DeviceContext> m_imContext;
	};
}

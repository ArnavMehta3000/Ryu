#pragma once
#include "Graphics/DX11/DX11Types.h"
#include "Graphics/Shared/Interface/IDevice.h"

namespace Ryu::Graphics
{
	class DX11Device : public IDevice
	{
	public:
		RYU_DECLARE_GFX_NATIVE_TYPE(DX11::IDX11Device);

		DX11Device(const DeviceCreateDesc& desc);
		~DX11Device();

	private:
		void ReportLiveObjects(bool releaseBeforeReporting = false) override;
		NativeObjectType GetNativeObject() const override;
		CreateSwapChainResult CreateSwapChain(const SwapChainDesc& desc) override;

		inline DX11::IDX11DeviceContext* GetImmediateContext() const { return m_imContext.Get(); }

		void InitDevice(const DeviceCreateDesc& desc);
	
	private:
		ComPtr<DX11::IDX11Device> m_device;
		ComPtr<DX11::IDX11DeviceContext> m_imContext;
	};
}

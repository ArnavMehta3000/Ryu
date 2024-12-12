#pragma once
#include "Graphics/DX11/DX11Types.h"
#include "Graphics/Shared/Interface/IDevice.h"

namespace Ryu::Graphics
{
	class DX11Device : public IDevice
	{
	public:
		DX11Device(const DeviceCreateDesc& desc);

	private:
		NativeObjectType GetNativeObject() const override;
		CreateSwapChainResult CreateSwapChain(const SwapChainDesc& desc) override;

		inline DX11::IDX11DeviceContext* GetImmediateContext() const { return m_imContext.Get(); }
	
	private:
		ComPtr<DX11::IDX11Device> m_device;
		ComPtr<DX11::IDX11DeviceContext> m_imContext;
	};
}

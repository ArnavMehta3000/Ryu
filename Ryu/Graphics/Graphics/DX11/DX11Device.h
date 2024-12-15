#pragma once
#include "Graphics/DX11/DX11Types.h"
#include "Graphics/DX11/IDX11Object.h"
#include "Graphics/Shared/Interface/IDevice.h"

namespace Ryu::Graphics
{
	class DX11Device : public IDevice, public IDX11Object
	{
	public:
		RYU_DECLARE_GFX_NATIVE_TYPE(DX11::IDX11Device);
		RYU_DECLARE_GFX_NATIVE_TYPE_OP(m_device.Get())

		DX11Device(const DeviceCreateDesc& desc);
		~DX11Device();

		inline DX11::IDX11DeviceContext* GetImmediateContext() const { return m_imContext.Get(); }

	private:
		void ReportLiveObjects(bool releaseBeforeReporting = false) override;
		NativeObjectType GetNativeObject() const override;
		CreateSwapChainResult CreateSwapChain(const SwapChainDesc& desc) const override;
		CreateCommandListResult CreateCommandList(const CommandListDesc& desc) const override;
		void ExecuteCommandList(const ICommandList* commandList) const override;


		void InitDevice(const DeviceCreateDesc& desc);
	
	private:
		ComPtr<NativeType> m_device;
		ComPtr<DX11::IDX11DeviceContext> m_imContext;
	};
}

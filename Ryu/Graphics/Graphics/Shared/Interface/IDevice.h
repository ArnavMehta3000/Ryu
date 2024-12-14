#pragma once
#include "Common/Result.h"
#include "Graphics/Shared/Interface/IGraphicsObject.h"
#include "Graphics/API.h"
#include <dxgi1_6.h>

namespace Ryu::Graphics
{
	class ISwapChain;
	struct SwapChainDesc;

	struct DeviceCreateDesc
	{
		API GraphicsAPI;
		bool EnableDebugLayer;
		bool EnableGPUBasedValidation;  // Only used for DirectX12
		bool EnableVSync;
	};

	class IDevice : public IGraphicsObject
	{
	protected:
		using CreateDeviceResult = Result<std::unique_ptr<IDevice>>;
		using CreateSwapChainResult = Result<std::unique_ptr<ISwapChain>>;

	public:
		virtual ~IDevice() = default;

		static CreateDeviceResult Create(const DeviceCreateDesc& desc);

		inline IDXGIAdapter4* GetAdapter() const { return m_adapter.Get(); }
		inline IDXGIFactory7* GetDXGIFactory() const { return m_dxgiFactory.Get(); }

		virtual void ReportLiveObjects(bool releaseBeforeReporting = false) = 0;
		virtual CreateSwapChainResult CreateSwapChain(const SwapChainDesc& desc) = 0;

	protected:
		bool InitializeDXGI(bool enableDebugLayer);
		void ShutdownDXGI();

	protected:
		ComPtr<IDXGIFactory7> m_dxgiFactory;
		ComPtr<IDXGIAdapter4> m_adapter;
	};
}

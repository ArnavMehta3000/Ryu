#pragma once
#include "Common/Result.h"
#include "GraphicsRHI/IGraphicsObject.h"
#include "GraphicsRHI/API.h"
#include <dxgi1_6.h>

namespace Ryu::Graphics
{
	class ISwapChain;
	class ICommandList;
	struct SwapChainDesc;
	struct CommandListDesc;

	struct DeviceCreateDesc
	{
		API GraphicsAPI;
		bool EnableDebugLayer;
		bool EnableGPUBasedValidation;  // Only used for DirectX12
		bool EnableVSync;
	};

	class IDevice : public IGraphicsObject
	{
	public:
		using CreateDeviceResult      = Result<std::unique_ptr<IDevice>>;
		using CreateSwapChainResult   = Result<std::unique_ptr<ISwapChain>>;
		using CreateCommandListResult = Result<std::unique_ptr<ICommandList>>;

	public:
		virtual ~IDevice() = default;

		void InitializeResource(IGraphicsObject* obj) const;

		inline IDXGIAdapter4* GetAdapter() const { return m_adapter.Get(); }
		inline IDXGIFactory7* GetDXGIFactory() const { return m_dxgiFactory.Get(); }

		virtual void ReportLiveObjects(bool releaseBeforeReporting = false) = 0;
		virtual CreateSwapChainResult CreateSwapChain(const SwapChainDesc& desc) const = 0;
		virtual CreateCommandListResult CreateCommandList(const CommandListDesc& desc) const = 0;
		virtual void ExecuteCommandList(const ICommandList* commandList) const = 0;

	protected:
		bool InitializeDXGI(bool enableDebugLayer);
		void ShutdownDXGI();

	protected:
		ComPtr<IDXGIFactory7> m_dxgiFactory;
		ComPtr<IDXGIAdapter4> m_adapter;
	};
}

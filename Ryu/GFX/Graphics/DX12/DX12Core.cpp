#include "DX12Core.h"
#include "Graphics/Config.h"
#include "Graphics/DX12/DX12Command.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics::DX12::Core
{
	namespace
	{
		LOG_CATEGORY(DX12Core);

		constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVEL{ D3D_FEATURE_LEVEL_11_0 };

		ID3D12Device8* g_device{ nullptr };
		IDXGIFactory7* g_dxgiFactory{ nullptr };
		DX12Command    g_gfxCommand;

		bool InitializationFailed()
		{
			LOG_ERROR(USE_LOG_CATEGORY(DX12Core), "DX12 initialization failed");

			Shutdown();
			return false;
		}

		IDXGIAdapter4* GetMainAdapter()
		{
			IDXGIAdapter4* adapter{ nullptr };

			for (u32 i = 0; g_dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				if (SUCCEEDED(::D3D12CreateDevice(adapter, MIN_FEATURE_LEVEL, __uuidof(ID3D12Device), nullptr)))
				{
					return adapter;
				}
				Release(adapter);
			}
			return nullptr;
		}

		D3D_FEATURE_LEVEL GetMaxFeatureLevel(IDXGIAdapter4* adapter)
		{
			// Use helper class to check for feature support
			ComPtr<ID3D12Device> device;
			DXCall(D3D12CreateDevice(adapter, MIN_FEATURE_LEVEL, IID_PPV_ARGS(&device)));

			CD3DX12FeatureSupport support;
			DXCall(support.Init(device.Get()));
			return support.MaxSupportedFeatureLevel();
		}
	}

	bool Init()
	{
		LOG_INFO(USE_LOG_CATEGORY(DX12Core), "Initializing DX12");

		if (g_device)
		{
			LOG_DEBUG(USE_LOG_CATEGORY(DX12Core), "DX12 already initialized");
			return InitializationFailed();
		}

		const GraphicsConfig& config = GraphicsConfig::Get();
		
		u32 dxgiCreationFlags = 0;
		if (config.EnableDebugLayer)
		{
			LOG_DEBUG(USE_LOG_CATEGORY(DX12Core), "Enabling graphics debug layer");
			dxgiCreationFlags |= DXGI_CREATE_FACTORY_DEBUG;

			ComPtr<ID3D12Debug6> debugController;
			DXCall(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));

			debugController->EnableDebugLayer();

			if (config.EnableGPUBasedValidation)
			{
				LOG_DEBUG(USE_LOG_CATEGORY(DX12Core), "Enabling GPU-based validation");
				debugController->SetEnableGPUBasedValidation(TRUE);
			}
		}

		DXCall(CreateDXGIFactory2(dxgiCreationFlags, IID_PPV_ARGS(&g_dxgiFactory)));

		// Get main adapter
		ComPtr<IDXGIAdapter4> adapter;
		adapter.Attach(GetMainAdapter());
		
		DEBUG_ASSERT(adapter, "Failed to get main adapter");
		if (!adapter) return InitializationFailed();

		//  Get max feature level
		const D3D_FEATURE_LEVEL maxFeatureLevel = GetMaxFeatureLevel(adapter.Get());
		DEBUG_ASSERT(maxFeatureLevel >= MIN_FEATURE_LEVEL);
		if (maxFeatureLevel < MIN_FEATURE_LEVEL) return InitializationFailed();

		// Create device
		DXCall(D3D12CreateDevice(adapter.Get(), maxFeatureLevel, IID_PPV_ARGS(&g_device)));
		DEBUG_ASSERT(g_device, "Failed to create device");
		DX12_NAME_OBJECT(g_device, L"Main Device");


		if (config.EnableDebugLayer)
		{
			ComPtr<ID3D12InfoQueue> infoQueue;
			DXCall(g_device->QueryInterface(IID_PPV_ARGS(&infoQueue)));

			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		}

		new (&g_gfxCommand) DX12Command(g_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
		if (!g_gfxCommand.GetCommandQueue()) return InitializationFailed();

		LOG_TRACE(USE_LOG_CATEGORY(DX12Core), "Initialized DX12");

		return true;
	}

	void Shutdown()
	{
		LOG_INFO(USE_LOG_CATEGORY(DX12Core), "Shutting down DX12");

		g_gfxCommand.Release();
		Release(g_dxgiFactory);

		if (const GraphicsConfig& config = GraphicsConfig::Get(); config.EnableDebugLayer)
		{
			{
				ComPtr<ID3D12InfoQueue> infoQueue;
				DXCall(g_device->QueryInterface(IID_PPV_ARGS(&infoQueue)));

				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
			}

			ComPtr<ID3D12DebugDevice> debugDevice;
			DXCall(g_device->QueryInterface(IID_PPV_ARGS(&debugDevice)));
			
			Release(g_device);

			DXCall(debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_SUMMARY | D3D12_RLDO_IGNORE_INTERNAL));
		}

		Release(g_device);

		LOG_TRACE(USE_LOG_CATEGORY(DX12Core), "Shutdown DX12");
	}

	void Render()
	{
		g_gfxCommand.BeginFrame();

		// Record commands
		ID3D12GraphicsCommandList6* cmdList{ g_gfxCommand.GetCommandList() };

		g_gfxCommand.EndFrame();
	}
}

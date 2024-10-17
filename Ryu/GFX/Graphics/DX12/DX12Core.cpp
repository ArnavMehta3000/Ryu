#include "DX12Core.h"
#include "Graphics/Common.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics::DX12::Core
{
	namespace
	{
		LOG_CATEGORY(DX12Core);

		constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVEL{ D3D_FEATURE_LEVEL_11_0 };

		ID3D12Device8* g_device{ nullptr };
		IDXGIFactory7* g_dxgiFactory{ nullptr };

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


		u32 dxgiCreationFlags = 0;
#if defined(RYU_BUILD_DEBUG)
		// Enable debug layer
		{
			ComPtr<ID3D12Debug> debugController;
			DXCall(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));

			debugController->EnableDebugLayer();

			dxgiCreationFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif

		DXCall(CreateDXGIFactory2(dxgiCreationFlags, IID_PPV_ARGS(&g_dxgiFactory)));

		ComPtr<IDXGIAdapter4> adapter;
		adapter.Attach(GetMainAdapter());
		
		DEBUG_ASSERT(adapter, "Failed to get main adapter");
		if (!adapter) return InitializationFailed();

		const D3D_FEATURE_LEVEL maxFeatureLevel = GetMaxFeatureLevel(adapter.Get());

		DEBUG_ASSERT(maxFeatureLevel >= MIN_FEATURE_LEVEL);
		if (maxFeatureLevel < MIN_FEATURE_LEVEL) return InitializationFailed();

		DXCall(D3D12CreateDevice(adapter.Get(), maxFeatureLevel, IID_PPV_ARGS(&g_device)));

		DEBUG_ASSERT(g_device, "Failed to create device");

		DX12_NAME_OBJECT(g_device, L"Main Device");

#if defined(RYU_BUILD_DEBUG)
		{
			ComPtr<ID3D12InfoQueue> infoQueue;
			DXCall(g_device->QueryInterface(IID_PPV_ARGS(&infoQueue)));

			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		}
#endif

		LOG_TRACE(USE_LOG_CATEGORY(DX12Core), "Initialized DX12");

		return true;
	}

	void Shutdown()
	{
		LOG_INFO(USE_LOG_CATEGORY(DX12Core), "Shutting down DX12");

		Release(g_dxgiFactory);

#if defined(RYU_BUILD_DEBUG)
		// Reset debug state
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
#endif

		Release(g_device);

		LOG_TRACE(USE_LOG_CATEGORY(DX12Core), "Shutdown DX12");
	}
}

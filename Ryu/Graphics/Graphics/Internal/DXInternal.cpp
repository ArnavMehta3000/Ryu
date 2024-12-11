#include "DXInternal.h"
#include "Graphics/API.h"
#include "Graphics/Config.h"

namespace Ryu::Graphics::Internal
{
	namespace
	{
		constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVEL{ D3D_FEATURE_LEVEL_11_0 };

		IDXGIFactory7* g_dxgiFactory{ nullptr };
		IDXGIAdapter4* g_adapter{ nullptr };
		D3D_FEATURE_LEVEL g_maxFeatureLevel;

		void CreateAdapterDX11()
		{
			D3D_FEATURE_LEVEL maxFeatureLevel;
			static constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVELS[] =
			{
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0
			};

			// --- Create adapter ---
			ComPtr<ID3D11Device> device;
			ComPtr<ID3D11DeviceContext> context;
			for (u32 i = 0; g_dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&g_adapter)) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				if (SUCCEEDED(::D3D11CreateDevice(
					g_adapter,
					D3D_DRIVER_TYPE_UNKNOWN,
					nullptr,
					D3D11_CREATE_DEVICE_BGRA_SUPPORT,
					MIN_FEATURE_LEVELS,
					_countof(MIN_FEATURE_LEVELS),
					D3D11_SDK_VERSION,
					&device,
					&maxFeatureLevel,
					&context)))
				{
					// Valid adapter found, break
					break;
				}
			}
		}

		void CreateAdapterDX12()
		{
			for (u32 i = 0; g_dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&g_adapter)) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				if (SUCCEEDED(::D3D12CreateDevice(g_adapter, MIN_FEATURE_LEVEL, __uuidof(ID3D12Device), nullptr)))
				{
					// Valid adapter found, break
					break;
				}
			}
		}

		void CaptureMaxFeatureLevelDX11()
		{

			static constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVELS[] =
			{
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0
			};

			ComPtr<ID3D11Device> device;
			ComPtr<ID3D11DeviceContext> context;
			HRESULT hr{ S_OK };

			DXCall(hr = ::D3D11CreateDevice(
				g_adapter,
				D3D_DRIVER_TYPE_UNKNOWN,
				nullptr,
				D3D11_CREATE_DEVICE_BGRA_SUPPORT,
				MIN_FEATURE_LEVELS,
				_countof(MIN_FEATURE_LEVELS),
				D3D11_SDK_VERSION,
				&device,
				&g_maxFeatureLevel,
				&context));
		}

		void CaptureMaxFeatureLevelDX12()
		{
			ComPtr<ID3D12Device> device;
			DXCall(D3D12CreateDevice(g_adapter, MIN_FEATURE_LEVEL, IID_PPV_ARGS(&device)));

			CD3DX12FeatureSupport support;
			DXCall(support.Init(device.Get()));
			g_maxFeatureLevel = support.MaxSupportedFeatureLevel();
		}
	
		void EnableDXGIDebugLayer()
		{
			const GraphicsConfig& config = GraphicsConfig::Get();
			if (!config.EnableDebugLayer)
			{
				return;
			}

			ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
			if (SUCCEEDED(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
			{
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
			}
		}
	}

	bool Init(API api)
	{
		::CoInitialize(NULL);
		EnableDXGIDebugLayer();

		// --- Create factory ---
		u32 dxgiCreationFlags = 0;
		if (GraphicsConfig::Get().EnableDebugLayer)
		{
			dxgiCreationFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
		DXCall(CreateDXGIFactory2(dxgiCreationFlags, IID_PPV_ARGS(&g_dxgiFactory)));
		
		
		switch (api)
		{
		case API::DirectX12:
			CreateAdapterDX11();
			CaptureMaxFeatureLevelDX12();
			break;

		case API::DirectX11:
			CreateAdapterDX12();
			CaptureMaxFeatureLevelDX11();
			break;
		}

		return true;
	}

	void Shutdown()
	{
		Release(g_adapter);
		Release(g_dxgiFactory);

		::CoUninitialize();
	}

	IDXGIFactory7* const GetFactory()
	{
		return g_dxgiFactory;
	}

	IDXGIAdapter4* const GetMainAdapter()
	{
		return g_adapter;
	}

	D3D_FEATURE_LEVEL GetMaxFeatureLevel()
	{
		return g_maxFeatureLevel;
	}

	std::pair<u32, u32> GetRefreshRate(DXGI_FORMAT format, u32 width, u32 height)
	{
		// Get main output window
		ComPtr<IDXGIOutput> output;
		HRESULT hr{ S_OK };
		if (SUCCEEDED(hr = g_adapter->EnumOutputs(0, &output)))
		{
			ComPtr<IDXGIOutput6> output6;
			if (SUCCEEDED(hr = output.As(&output6)))
			{
				u32 numModes{ 0 };
				DXGI_MODE_DESC1* displayModes = NULL;
				if (SUCCEEDED(hr = output6->GetDisplayModeList1(format, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr)))
				{
					displayModes = new DXGI_MODE_DESC1[numModes]; 
					if (SUCCEEDED(hr = output6->GetDisplayModeList1(format, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModes)))
					{
						for (u32 i = 0; i < numModes; i++)
						{
							if (displayModes[i].Width == width &&
								displayModes[i].Height == height)
							{
								return std::make_pair(
									displayModes[i].RefreshRate.Numerator, 
									displayModes[i].RefreshRate.Denominator);
							}
						}
					}
				}

				delete[] displayModes;
			}
		}

		return std::make_pair(60u, 1u);
	}

	bool HasTearingSupport()
	{

		BOOL allowTearing = FALSE;
		if (SUCCEEDED(g_dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing))))
		{
			allowTearing = TRUE;
		}

		return allowTearing == TRUE;
	}
}

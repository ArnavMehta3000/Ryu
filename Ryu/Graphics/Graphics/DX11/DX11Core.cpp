#include "DX11Core.h"
#include "Graphics/Config.h"
#include "Graphics/Internal/DXInternal.h"
#include "Graphics/DX11/DX11Surface.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics::DX11::Core
{
	namespace
	{
		RYU_LOG_CATEGORY(DX11Core);

		constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVEL{ D3D_FEATURE_LEVEL_11_0 };
		constexpr DXGI_FORMAT DEFAULT_RENDER_TARGET_FORMAT{ DXGI_FORMAT_R8G8B8A8_UNORM_SRGB };
		
		ID3D11Device5*               g_device{ nullptr };
		ID3D11DeviceContext4*        g_imContext{ nullptr };
		std::unique_ptr<DX11Surface> g_surface{ nullptr };

		bool InitializationFailed()
		{
			LOG_ERROR(RYU_USE_LOG_CATEGORY(DX11Core), "DX11 initialization failed");

			Shutdown();
			return false;
		}

		void ReportLiveObjects(bool releaseDevice = true)
		{
			LOG_DEBUG(RYU_USE_LOG_CATEGORY(DX11Core), "~~~DX11 Report Live Objects~~~");
			
			ComPtr<ID3D11Debug> debug;
			if (SUCCEEDED(g_device->QueryInterface(IID_PPV_ARGS(&debug))))
			{
				if (releaseDevice)
				{
					Release(g_device);
				}

				debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL | D3D11_RLDO_SUMMARY);
			}
		}
	}

	bool Init()
	{
		LOG_INFO(RYU_USE_LOG_CATEGORY(DX11Core), "Initializing DX12");

		if (g_device)
		{
			LOG_DEBUG(RYU_USE_LOG_CATEGORY(DX11Core), "DX11 already initialized");
			return InitializationFailed();
		}

		const GraphicsConfig& config = GraphicsConfig::Get();
		u32 deviceCreationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		if (config.EnableDebugLayer)
		{
			LOG_DEBUG(RYU_USE_LOG_CATEGORY(DX11Core), "Enabling graphics debug layer");
			deviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
		}

		// Device crea
		{
			IDXGIAdapter4* const adapter = Graphics::Internal::GetMainAdapter();

			DEBUG_ASSERT(adapter, "Failed to get main adapter");
			if (!adapter) return InitializationFailed();

			const D3D_FEATURE_LEVEL maxFeatureLevel = Graphics::Internal::GetMaxFeatureLevel();
			DEBUG_ASSERT(maxFeatureLevel >= MIN_FEATURE_LEVEL);
			if (maxFeatureLevel < MIN_FEATURE_LEVEL) return InitializationFailed();

			// Device creation block
			{
				ComPtr<ID3D11Device> device;
				ComPtr<ID3D11DeviceContext> deviceContext;
				DXCall(::D3D11CreateDevice(
					adapter,
					D3D_DRIVER_TYPE_UNKNOWN,
					NULL,
					deviceCreationFlags,
					&maxFeatureLevel,
					1,
					D3D11_SDK_VERSION,
					&device,
					NULL,
					&deviceContext));

				DXCall(device->QueryInterface(IID_PPV_ARGS(&g_device)));
				DXCall(deviceContext->QueryInterface(IID_PPV_ARGS(&g_imContext)));
			}
		}

		// Enable debug layer
		if (config.EnableDebugLayer)
		{
			ComPtr<ID3D11Debug> debug;
			if (SUCCEEDED(g_device->QueryInterface(IID_PPV_ARGS(&debug))))
			{
				ComPtr<ID3D11InfoQueue> infoQueue;
				if (SUCCEEDED(debug->QueryInterface(IID_PPV_ARGS(&infoQueue))))
				{
#if defined(RYU_BUILD_DEBUG)
					infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
					infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif

					D3D11_MESSAGE_ID hide[] =
					{
						D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
					};

					D3D11_INFO_QUEUE_FILTER filter = {};
					filter.DenyList.NumIDs = _countof(hide);
					filter.DenyList.pIDList = hide;
					infoQueue->AddStorageFilterEntries(&filter);
				}
			}
		}

		DX11_NAME_OBJECT(g_device, "Main Device");
		DX11_NAME_OBJECT(g_imContext, "Immediate Device Context");

		return true;
	}

	void Shutdown()
	{
		g_surface.reset();

		g_imContext->ClearState();
		g_imContext->Flush();

		Release(g_imContext);

		if (GraphicsConfig::Get().EnableDebugLayer)
		{
			ReportLiveObjects();
		}

		Release(g_device);
	}

	ISurface* CreateSurface(App::WindowBase* window)
	{
		return nullptr;
	}

	void OnResizeSurface(u32 width, u32 height)
	{

	}

	void RenderSurface()
	{

	}

	ID3D11Device5* const GetDevice()
	{
		return g_device;
	}

	ID3D11DeviceContext4* const GetImContext()
	{
		return g_imContext;
	}

	DXGI_FORMAT GetDefaultRenderTargetFormat()
	{
		return DEFAULT_RENDER_TARGET_FORMAT;
	}
}

#include "DX11Device.h"
#include "Graphics/Config.h"
#include "Graphics/Shared/Logging.h"
#include "Graphics/Shared/D3DUtil.h"
#include "Graphics/DXGI/DXGISwapChain.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics
{
	DX11Device::DX11Device(const DeviceCreateDesc& desc)
	{
		DEBUG_ASSERT(desc.GraphicsAPI == API::DirectX11, "DeviceCreateDesc graphics API must be DirectX11!");
		
		InitDevice(desc);
	}

	DX11Device::~DX11Device()
	{
		ShutdownDXGI();

		m_imContext.Reset();

		// Release the device
		if (GraphicsConfig::Get().EnableDebugLayer)
		{
			ReportLiveObjects(true);
		}
		else
		{
			m_device.Reset();
		}
	}

	void DX11Device::ReportLiveObjects(bool releaseBeforeReporting)
	{
		ComPtr<ID3D11Debug> debugDevice;
		if (SUCCEEDED(m_device.As(&debugDevice)))
		{
			if (releaseBeforeReporting)
			{
				// Release the device after acquring its debug interface
				m_device.Reset();
			}

			LOG_DEBUG(Internal::GraphicsDebugLog, "~~~DX11 Report Live Objects~~~");
			debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL | D3D11_RLDO_SUMMARY);
		}
	}

	NativeObjectType DX11Device::GetNativeObject() const
	{
		return m_device.Get();
	}
	
	IDevice::CreateSwapChainResult DX11Device::CreateSwapChain(const SwapChainDesc& desc)
	{
		DEBUG_ASSERT(m_device, "DX11Device is not initialized!");
		DEBUG_ASSERT(m_dxgiFactory, "DXGI factory is not initialized!");
		DEBUG_ASSERT(m_adapter, "Adapter is not initialized!");

		u32 numerator = 60;
		u32 denominator = 1;

		const DXGI_FORMAT format = Utils::ToNonSRGBFormat(Utils::GetFormat(desc.Format));

		ComPtr<IDXGIOutput> output;
		for (UINT i = 0; m_adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			UINT numModes = 0;
			if (FAILED(output->GetDisplayModeList(format, 0, &numModes, nullptr)) || numModes == 0)
			{
				continue;
			}

			std::vector<DXGI_MODE_DESC> modes(numModes);
			if (FAILED(output->GetDisplayModeList(format, 0, &numModes, modes.data())))
			{
				continue;
			}

			for (const auto& mode : modes)
			{
				if (mode.Width == desc.Width &&
					mode.Height == desc.Height &&
					mode.Format == format)
				{
					UINT currentRate = mode.RefreshRate.Numerator / mode.RefreshRate.Denominator;
					UINT bestRate = numerator / denominator;

					if (currentRate > bestRate)
					{
						numerator = mode.RefreshRate.Numerator;
						denominator = mode.RefreshRate.Denominator;
					}
				}
			}
			output.Reset();
		}

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc1{};
		swapChainDesc1.Width              = desc.Width;
		swapChainDesc1.Height             = desc.Height;
		swapChainDesc1.Format             = format;
		swapChainDesc1.BufferCount        = desc.BufferCount;
		swapChainDesc1.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc1.SampleDesc.Count   = desc.SampleCount;
		swapChainDesc1.SampleDesc.Quality = desc.SampleQuality;
		swapChainDesc1.SwapEffect         = Utils::GetSwapEffect(desc.SwapEffect);
		swapChainDesc1.AlphaMode          = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc1.Scaling            = DXGI_SCALING_STRETCH;
		swapChainDesc1.Flags              = desc.Flags;
		swapChainDesc1.Stereo             = FALSE;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc{};
		fsDesc.RefreshRate.Numerator   = numerator;
		fsDesc.RefreshRate.Denominator = denominator;
		fsDesc.Windowed                = desc.Windowed;
		fsDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		fsDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

		ComPtr<IDXGISwapChain1> swapChain1;
		HRESULT hr = m_dxgiFactory->CreateSwapChainForHwnd(
			m_device.Get(),
			desc.WindowHandle,
			&swapChainDesc1,
			&fsDesc,
			nullptr,  // Restrict to output window
			&swapChain1
		);

		if (FAILED(hr))
		{
			DXCall(hr);
			return MakeResultError{ "Failed to create DXGI swap chain!" };
		}

		// Disable Alt+Enter fullscreen transitions handled by DXGI
		DXCall(m_dxgiFactory->MakeWindowAssociation(desc.WindowHandle, DXGI_MWA_NO_ALT_ENTER));

		ComPtr<IDXGISwapChain4> swapChain4;
		DXCall(swapChain1.As(&swapChain4));
		DX11_NAME_OBJECT(swapChain4.Get(), "DXGI SwapChain");  // DXGI object use the DX11 naming convention

		return std::make_unique<DXGISwapChain>(swapChain4.Detach());
	}
	
	void DX11Device::InitDevice(const DeviceCreateDesc& desc)
	{
		static constexpr std::array<D3D_FEATURE_LEVEL, 4> featureLevels =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0
		};

		u32 createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		if (desc.EnableDebugLayer)
		{
			createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
			LOG_DEBUG(Internal::GraphicsDebugLog, "Graphics debug layer enabled");
		}

		if (!InitializeDXGI(desc.EnableDebugLayer))
		{
			LOG_ERROR(Internal::GraphicsDebugLog, "Failed to initialize DXGI!");
			return;
		}

		D3D_FEATURE_LEVEL achievedFeatureLevel;
		{
			ComPtr<ID3D11Device> device;
			ComPtr<ID3D11DeviceContext> imContext;

			HRESULT hr = ::D3D11CreateDevice(
				m_adapter.Get(),
				D3D_DRIVER_TYPE_UNKNOWN,
				nullptr,
				createDeviceFlags,
				featureLevels.data(),
				static_cast<u32>(featureLevels.size()),
				D3D11_SDK_VERSION,
				&device,
				&achievedFeatureLevel,
				&imContext
			);

			// Get updated interface for device
			DXCall(device.As(&m_device));
			DXCall(imContext.As(&m_imContext));

			DX11_NAME_OBJECT(m_device, "DX11 Main Device");
			DX11_NAME_OBJECT(m_imContext, "DX11 Immediate Context");
		}

		DEBUG_ASSERT(m_device, "Failed to create DX11 device!");
		DEBUG_ASSERT(m_imContext, "Failed to create DX11 immediate context!");

		if (desc.EnableDebugLayer)
		{
			ComPtr<ID3D11Debug> debugDevice;
			if (SUCCEEDED(m_device.As(&debugDevice)))
			{
				ComPtr<ID3D11InfoQueue> infoQueue;
				if (SUCCEEDED(debugDevice.As(&infoQueue)))
				{
					infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
					infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, TRUE);

					D3D11_MESSAGE_ID hide[] = 
					{
						D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS
					};

					D3D11_INFO_QUEUE_FILTER filter{};
					filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
					filter.DenyList.pIDList = hide;
					infoQueue->AddStorageFilterEntries(&filter);
				}
			}
		}
	}
}

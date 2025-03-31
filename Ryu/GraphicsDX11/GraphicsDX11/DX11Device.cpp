#include "DX11Device.h"
#include "GraphicsRHI/Config.h"
#include "GraphicsRHI/IRenderer.h"
#include "GraphicsRHI/Utils/Logging.h"
#include "GraphicsRHI/Utils/D3DUtil.h"
#include "GraphicsDX11/DX11SwapChain.h"
#include "GraphicsDX11/DX11CommandList.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics::DX11
{
	DX11Device::DX11Device()
	{
		DEBUG_ASSERT((GraphicsConfig::Get().GraphicsAPI.Get() == API::DirectX11),
			"DeviceCreateDesc graphics API must be DirectX11!");
		
		InitDevice();
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
				m_device.Reset();  // Release the device after acquring its debug interface
			}

			LOG_DEBUG(Internal::GraphicsDebugLog, "~~~DX11 Report Live Objects~~~");
			debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL | D3D11_RLDO_SUMMARY);
		}
	}
	
	IDevice::CreateSwapChainResult DX11Device::CreateSwapChain(const SwapChainDesc& desc) const
	{
		DEBUG_ASSERT(m_device, "DX11Device is not initialized!");
		DEBUG_ASSERT(m_dxgiFactory, "DXGI factory is not initialized!");
		DEBUG_ASSERT(m_adapter, "Adapter is not initialized!");

		if (!m_dxgiFactory || !m_adapter || !m_adapter)
		{
			return MakeResultError{ "Failed to create swap chain" };
		}

		return std::make_unique<DX11SwapChain>(this, desc);
	}
	
	IDevice::CreateCommandListResult DX11Device::CreateCommandList(const CommandListDesc& desc) const
	{
		DEBUG_ASSERT(m_device, "DX11Device is not initialized!");

		auto ptr = std::make_unique<DX11CommandList>(this, desc);
		
		return std::move(ptr);
	}

	void DX11Device::ExecuteCommandList(const ICommandList* commandList) const
	{
		if (DX11CommandList::NativeType* cmdList = RYU_GET_GFX_NATIVE_TYPE(commandList, DX11CommandList::NativeType))
		{
			m_imContext->ExecuteCommandList(cmdList, FALSE);
		}
	}

	void DX11Device::InitDevice()
	{
		static constexpr std::array<D3D_FEATURE_LEVEL, 4> featureLevels =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0
		};
		const GraphicsConfig& config = GraphicsConfig::Get();

		u32 createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		if (config.EnableDebugLayer)
		{
			createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
			LOG_DEBUG(Internal::GraphicsDebugLog, "Graphics debug layer enabled");
		}

		if (!InitializeDXGI(config.EnableDebugLayer))
		{
			LOG_ERROR(Internal::GraphicsDebugLog, "Failed to initialize DXGI!");
			return;
		}

		D3D_FEATURE_LEVEL achievedFeatureLevel;
		{
			ComPtr<ID3D11Device> device;
			ComPtr<ID3D11DeviceContext> imContext;

			DXCall(D3D11CreateDevice(
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
			));

			// Get updated interface for device
			DXCall(device.As(&m_device));
			DXCall(imContext.As(&m_imContext));

			DX11_NAME_OBJECT(m_device, "DX11 Main Device");
			DX11_NAME_OBJECT(m_imContext, "DX11 Immediate Context");
		}

		DEBUG_ASSERT(m_device, "Failed to create DX11 device!");
		DEBUG_ASSERT(m_imContext, "Failed to create DX11 immediate context!");

		if (config.EnableDebugLayer)
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

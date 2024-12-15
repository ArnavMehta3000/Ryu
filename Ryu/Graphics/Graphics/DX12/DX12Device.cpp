#include "DX12Device.h"
#include "Graphics/Config.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shared/Logging.h"
#include "Graphics/Shared/D3DUtil.h"
#include "Graphics/DXGI/DXGISwapChain.h"
#include "Graphics/DX12/DX12CommandList.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics
{
	DX12Device::DX12Device(const DeviceCreateDesc& desc)
	{
		DEBUG_ASSERT(desc.GraphicsAPI == API::DirectX12, "DeviceCreateDesc graphics API must be DirectX12!");

		InitDevice(desc);
	}

	DX12Device::~DX12Device()
	{
		ShutdownDXGI();

		m_cmdQueue.Reset();
		
		if (GraphicsConfig::Get().EnableDebugLayer)
		{
			ReportLiveObjects(true);
		}
		else
		{
			m_device.Reset();
		}
	}
	
	void DX12Device::ReportLiveObjects(bool releaseBeforeReporting)
	{
		ComPtr<ID3D12DebugDevice> debugDevice;
		if (SUCCEEDED(m_device.As(&debugDevice)))
		{
			if (releaseBeforeReporting)
			{
				m_device.Reset();  // Release the device after acquring its debug interface
			}

			LOG_DEBUG(Internal::GraphicsDebugLog, "~~~DX12 Report Live Objects~~~");
			debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_SUMMARY | D3D12_RLDO_IGNORE_INTERNAL);
		}
	}

	NativeObjectType DX12Device::GetNativeObject() const
	{
		return m_device.Get();
	}
	
	IDevice::CreateSwapChainResult DX12Device::CreateSwapChain(const SwapChainDesc& desc) const
	{
		DEBUG_ASSERT(m_dxgiFactory, "DXGI factory is not initialized!");
		DEBUG_ASSERT(m_adapter, "Adapter is not initialized!");
		DEBUG_ASSERT(m_cmdQueue, "Adapter is not initialized!");

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
			m_cmdQueue.Get(),
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

		return std::make_unique<DXGISwapChain>(desc, swapChain4.Detach());
	}

	IDevice::CreateCommandListResult DX12Device::CreateCommandList(const CommandListDesc& desc) const
	{
		auto ptr = std::make_unique<DX12CommandList>(*this, desc);
		
		DX12CommandList::NativeType* nativePtr = *ptr.get();
		ptr->SetName(nativePtr, "DX12 Command List");
		InitializeResource(ptr.get());
		
		return std::move(ptr);
	}

	void DX12Device::ExecuteCommandList(const ICommandList* commandList) const
	{
		if (DX12CommandList::NativeType* cmdList = RYU_GET_GFX_NATIVE_TYPE(commandList, DX12CommandList::NativeType))
		{
			DX12::IDX12CommandList* const cmdLists[] = { cmdList };
			m_cmdQueue->ExecuteCommandLists(1, cmdLists);

			// Signal and wait for execution if immediate sync is needed
			if (auto dx12CmdList = dynamic_cast<const DX12CommandList*>(commandList))
			{
				dx12CmdList->Signal();
			}
		}
	}
	
	void DX12Device::InitDevice(const DeviceCreateDesc& desc)
	{
		if (desc.EnableDebugLayer)
		{
			ComPtr<ID3D12Debug6> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();
				LOG_DEBUG(Internal::GraphicsDebugLog, "Graphics debug layer enabled");

				if (desc.EnableGPUBasedValidation)
				{
					LOG_DEBUG(Internal::GraphicsDebugLog, "Enabling GPU-based validation");
					debugController->SetEnableGPUBasedValidation(TRUE);
				}
			}
		}

		if (!InitializeDXGI(desc.EnableDebugLayer))
		{
			LOG_ERROR(Internal::GraphicsDebugLog, "Failed to initialize DXGI!");
			return;
		}

		DEBUG_ASSERT(m_dxgiFactory, "DXGI factory is not initialized!");
		DEBUG_ASSERT(m_adapter, "Adapter is not initialized!");

		DXCall(D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		queueDesc.NodeMask = 0;

		DXCall(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_cmdQueue)));

		// Set up debug messages if enabled
		if (desc.EnableDebugLayer)
		{
			ComPtr<ID3D12InfoQueue> infoQueue;
			if (SUCCEEDED(m_device.As(&infoQueue)))
			{
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);

				D3D12_MESSAGE_ID hide[] = 
				{
					D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
					D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
				};

				D3D12_INFO_QUEUE_FILTER filter = {};
				filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
				filter.DenyList.pIDList = hide;
				infoQueue->AddStorageFilterEntries(&filter);
			}
		}

		DX12_NAME_OBJECT(m_device.Get(), L"Main Device");
		DX12_NAME_OBJECT(m_cmdQueue.Get(), L"Main Command Queue");
	}
}

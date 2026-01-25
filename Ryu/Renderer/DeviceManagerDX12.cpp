#include "Renderer/DeviceManagerDX12.h"
#include "Core/Utils/StringConv.h"
#include "Core/Logging/Logger.h"

#define HR_RETURN(hr) if (FAILED(hr)) { return false; }


namespace Ryu::Graphics
{
	std::string DeviceManagerDX12::GetAdapterName(const DXGI_ADAPTER_DESC& desc)
	{
		return Ryu::Utils::ToNarrowStr(desc.Description);
	}
	
	bool DeviceManagerDX12::EnumerateAdapters(std::vector<AdapterInfo>& outAdapters)
	{
		if (!m_dxgiFactory)
		{
			return false;
		}

		outAdapters.clear();

		while (true)
		{
			nvrhi::RefCountPtr<IDXGIAdapter> adapter;

			HRESULT hr = m_dxgiFactory->EnumAdapters(u32(outAdapters.size()), &adapter);
			if (FAILED(hr))
			{
				return false;
			}

			DXGI_ADAPTER_DESC desc;
			hr = adapter->GetDesc(&desc);
			if (FAILED(hr))
			{
				return false;
			}

			AdapterInfo adapterInfo
			{
				.Name                 = GetAdapterName(desc),
				.VendorID             = desc.VendorId,
				.DeviceID             = desc.DeviceId,
				.DedicatedVideoMemory = desc.DedicatedVideoMemory,
				.DXGIAdapter          = adapter
			};

			AdapterInfo::AdapterLUID luid;			
			static_assert(luid.size() == sizeof(desc.AdapterLuid));
			
			std::memcpy(luid.data(), &desc.AdapterLuid, luid.size());
			adapterInfo.LUID = luid;

			outAdapters.push_back(std::move(adapterInfo));
		}
	}

	nvrhi::ITexture* DeviceManagerDX12::GetCurrentBackBuffer()
	{
		return m_rhiSwapChainBuffers[m_swapChain->GetCurrentBackBufferIndex()];
	}

	nvrhi::ITexture* DeviceManagerDX12::GetBackBuffer(u32 index)
	{
		return (index < (u32)m_rhiSwapChainBuffers.size()) ? m_rhiSwapChainBuffers[index] : nullptr;
	}

	u32 DeviceManagerDX12::GetCurrentBackBufferIndex()
	{
		return m_swapChain->GetCurrentBackBufferIndex();
	}

	u32 DeviceManagerDX12::GetBackBufferCount()
	{
		return m_swapChainDesc.BufferCount;
	}

	void DeviceManagerDX12::ReportLiveObjects()
	{
		nvrhi::RefCountPtr<IDXGIDebug> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			DXGI_DEBUG_RLO_FLAGS flags = (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_DETAIL);
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, flags);
		}
	}
	
	bool DeviceManagerDX12::CreateInstanceInternal()
	{
		if (!m_dxgiFactory)
		{
			HRESULT hr = CreateDXGIFactory2(m_createInfo.EnableDebugRuntime ? DXGI_CREATE_FACTORY_DEBUG : 0, IID_PPV_ARGS(&m_dxgiFactory));

			if (FAILED(hr))
			{
				RYU_LOG_ERROR("Failed to create DXGI factory!");
				return false;
			}
		}
		
		return true;
	}
	
	bool DeviceManagerDX12::CreateDevice()
	{
		// Check for debug layer
		if (m_createInfo.EnableDebugRuntime)
		{
			nvrhi::RefCountPtr<ID3D12Debug> debug;
			if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
			{
				debug->EnableDebugLayer();
			}
			else
			{
				RYU_LOG_WARN("Unable to enable DX12 debug layer");
			}
		}

		// Check for GPU validation
		if (m_createInfo.EnableGPUValidationDX12)
		{
			nvrhi::RefCountPtr<ID3D12Debug3> debug;
			if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
			{
				debug->SetEnableGPUBasedValidation(true);
			}
			else
			{
				RYU_LOG_WARN("Unable to enable DX12 GPU validation");
			}
		}

		// Check for valid adapter
		i32 adapterIndex = m_createInfo.AdapterIndex;
		if (adapterIndex < 0) { adapterIndex = 0; }

		if (FAILED(m_dxgiFactory->EnumAdapters(adapterIndex, &m_dxgiAdapter)))
		{
			if (adapterIndex == 0)
			{
				RYU_LOG_ERROR("Cannot finad any DXGI adapters in the system!");
			}
			else
			{
				RYU_LOG_ERROR("Specified DXGI adapter {} does not exist", adapterIndex);
			}

			return false;
		}

		{
			DXGI_ADAPTER_DESC aDesc;
			m_dxgiAdapter->GetDesc(&aDesc);
			m_rendererString = GetAdapterName(aDesc);
		}

		// Create device
		HRESULT hr = D3D12CreateDevice(m_dxgiAdapter, m_createInfo.FeatureLevel, IID_PPV_ARGS(&m_device));
		if (FAILED(hr))
		{
			RYU_LOG_ERROR("Failed to create D3D12 device. Error: {}", hr);
			return false;
		}

		// Set up message filter layer
		if (m_createInfo.EnableDebugRuntime)
		{
			nvrhi::RefCountPtr<ID3D12InfoQueue> infoQueue;
			m_device->QueryInterface(&infoQueue);

			if (infoQueue)
			{
#if defined (RYU_BUILD_DEBUG)
				if (m_createInfo.EnableWarningsAsErrors)
				{
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
				}
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif

				D3D12_MESSAGE_ID disableMessageIDs[] = 
				{
					D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
					D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
					D3D12_MESSAGE_ID_COMMAND_LIST_STATIC_DESCRIPTOR_RESOURCE_DIMENSION_MISMATCH, // descriptor validation doesn't understand acceleration structures
					D3D12_MESSAGE_ID_CREATERESOURCE_STATE_IGNORED,                               // NGX currently generates benign resource creation warnings
				};

				D3D12_INFO_QUEUE_FILTER filter{};
				filter.DenyList.pIDList        = disableMessageIDs;
				filter.DenyList.NumIDs         = sizeof(disableMessageIDs) / sizeof(disableMessageIDs[0]);
				infoQueue->AddStorageFilterEntries(&filter);
			}
		}

		// Create command queue
		D3D12_COMMAND_QUEUE_DESC queueDesc
		{
			.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = 0,
			.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 1
		};
		hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_graphicsQueue));
		HR_RETURN(hr);

		if (m_createInfo.EnableComputeQueue)
		{
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
			hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_computeQueue));
			HR_RETURN(hr);
		}

		if (m_createInfo.EnableCopyQueue)
		{
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
			hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_copyQueue));
			HR_RETURN(hr);
		}

		nvrhi::d3d12::DeviceDesc deviceDesc;
		deviceDesc.errorCB                   = m_createInfo.MessageCallback ? m_createInfo.MessageCallback : &DefaultMessageCallback::Get();
		deviceDesc.pDevice                   = m_device;
		deviceDesc.pGraphicsCommandQueue     = m_graphicsQueue;
		deviceDesc.pComputeCommandQueue      = m_computeQueue;
		deviceDesc.pCopyCommandQueue         = m_copyQueue;
		deviceDesc.logBufferLifetime         = m_createInfo.LogBufferLifetime;
		deviceDesc.enableHeapDirectlyIndexed = m_createInfo.EnableHeapDirectlyIndexedDX12;

		m_nvrhiDevice = nvrhi::d3d12::createDevice(deviceDesc);

		if (m_createInfo.EnableNvrhiValidationLayer)
		{
			m_nvrhiDevice = nvrhi::validation::createValidationLayer(m_nvrhiDevice);
		}

		return true;
	}
	
	bool DeviceManagerDX12::CreateSwapChain()
	{
		HRESULT hr = E_FAIL;

		RECT clientRect;
		::GetClientRect(m_hWnd, &clientRect);
		u32 width  = clientRect.right - clientRect.left;
		u32 height = clientRect.bottom - clientRect.top;

		ZeroMemory(&m_swapChainDesc, sizeof(m_swapChainDesc));
		m_swapChainDesc.Width              = width;
		m_swapChainDesc.Height             = height;
		m_swapChainDesc.SampleDesc.Count   = m_createInfo.SwapChainSampleCount;
		m_swapChainDesc.SampleDesc.Quality = m_createInfo.SwapChainSampleQuality;
		m_swapChainDesc.BufferUsage        = m_createInfo.SwapChainUsage;
		m_swapChainDesc.BufferCount        = m_createInfo.SwapChainBufferCount;
		m_swapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		m_swapChainDesc.Flags              = 0;  // NOTE: Could consider adding flag for DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH

		switch (m_createInfo.SwapChainFormat)  // NOLINT(clang-diagnostic-switch-enum)
		{
			using enum nvrhi::Format;
		case SRGBA8_UNORM : m_swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                                break;
		case SBGRA8_UNORM : m_swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;                                break;
		default           : m_swapChainDesc.Format = nvrhi::d3d12::convertFormat(m_createInfo.SwapChainFormat); break;
		}

		nvrhi::RefCountPtr<IDXGIFactory5> dxgiFactory5;
		if (SUCCEEDED(m_dxgiFactory->QueryInterface(IID_PPV_ARGS(&dxgiFactory5))))
		{
			BOOL supported = 0;
			if (SUCCEEDED(dxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &supported, sizeof(supported))))
			{
				m_isTearingSupported = (supported != 0);
			}
		}

		if (m_isTearingSupported)
		{
			m_swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		}

		m_fullScreenDesc = {};
		m_fullScreenDesc.RefreshRate.Numerator   = m_createInfo.RefreshRate;
		m_fullScreenDesc.RefreshRate.Denominator = 1;
		m_fullScreenDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
		m_fullScreenDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
		m_fullScreenDesc.Windowed                = TRUE;  // NOTE: Handle fullscreening eventually here

		nvrhi::RefCountPtr<IDXGISwapChain1> swapChain1;
		hr = m_dxgiFactory->CreateSwapChainForHwnd(m_graphicsQueue, m_hWnd, &m_swapChainDesc, &m_fullScreenDesc, nullptr, &swapChain1);
		HR_RETURN(hr)

		hr = swapChain1->QueryInterface(IID_PPV_ARGS(&m_swapChain));
		HR_RETURN(hr)

		if (!CreateRenderTargets())
		{
			return false;
		}

		hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_frameFence));
		HR_RETURN(hr)

		for (u32 bufferIndex = 0; bufferIndex < m_swapChainDesc.BufferCount; bufferIndex++)
		{
			m_frameFenceEvents.push_back(::CreateEvent(nullptr, FALSE, TRUE, nullptr));
		}

		return true;
	}
	
	void DeviceManagerDX12::DestroyDeviceAndSwapChain()
	{
		m_rhiSwapChainBuffers.clear();
		m_rendererString.clear();

		ReleaseRenderTargets();
		
		m_nvrhiDevice = nullptr;

		for (auto fenceEvent : m_frameFenceEvents)
		{
			::WaitForSingleObject(fenceEvent, INFINITE);
			::CloseHandle(fenceEvent);
		}

		m_frameFenceEvents.clear();

		if (m_swapChain)
		{
			m_swapChain->SetFullscreenState(FALSE, nullptr);
		}

		m_swapChainBuffers.clear();

		m_frameFence    = nullptr;
		m_swapChain     = nullptr;
		m_graphicsQueue = nullptr;
		m_computeQueue  = nullptr;
		m_copyQueue     = nullptr;
		m_device        = nullptr;
	}

	void DeviceManagerDX12::ResizeSwapChain()
	{
		ReleaseRenderTargets();

		if (!m_nvrhiDevice || !m_swapChain)
		{
			return;
		}

		const HRESULT hr = m_swapChain->ResizeBuffers(
			m_createInfo.SwapChainBufferCount, 
			m_createInfo.BackBufferWidth, 
			m_createInfo.BackBufferHeight, 
			m_swapChainDesc.Format, 
			m_swapChainDesc.Flags);
		
		if (FAILED(hr))
		{
			RYU_LOG_FATAL("Failed to resize swapchain!");
		}

		if (!CreateRenderTargets())
		{
			RYU_LOG_FATAL("Failed to create render targets!");
		}
	}

	bool DeviceManagerDX12::BeginFrame()
	{
		DXGI_SWAP_CHAIN_DESC1 newSwapChainDesc;
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC newFullScreenDesc;
	
		// Trigger a resize if the window state has changed
		if (SUCCEEDED(m_swapChain->GetDesc1(&newSwapChainDesc)) 
			&& SUCCEEDED(m_swapChain->GetFullscreenDesc(&newFullScreenDesc)))
		{
			if (m_fullScreenDesc.Windowed != newFullScreenDesc.Windowed)
			{
				BackBufferResizing();

				m_fullScreenDesc              = newFullScreenDesc;
				m_swapChainDesc               = newSwapChainDesc;
				m_createInfo.BackBufferWidth  = newSwapChainDesc.Width;
				m_createInfo.BackBufferHeight = newSwapChainDesc.Height;

				ResizeSwapChain();
				BackBufferResized();
			}

		}

		u32 bufferIndex = m_swapChain->GetCurrentBackBufferIndex();
		::WaitForSingleObject(m_frameFenceEvents[bufferIndex], INFINITE);

		return true;
	}

	bool DeviceManagerDX12::Present()
	{
		u32 bufferIndex = m_swapChain->GetCurrentBackBufferIndex();

		u32 presentFlags = 0;
		if (!m_createInfo.EnableVSync && m_fullScreenDesc.Windowed && m_isTearingSupported)
		{
			presentFlags |= DXGI_PRESENT_ALLOW_TEARING;
		}

		HRESULT result = m_swapChain->Present(m_createInfo.EnableVSync ? 1 : 0, presentFlags);

		m_frameFence->SetEventOnCompletion(m_frameCount, m_frameFenceEvents[bufferIndex]);
		m_graphicsQueue->Signal(m_frameFence, m_frameCount);
		m_frameCount++;
		
		return SUCCEEDED(result);
	}

	void DeviceManagerDX12::Shutdown()
	{
		DeviceManager::Shutdown();

		m_dxgiAdapter = nullptr;
		m_dxgiFactory = nullptr;

		if (m_createInfo.EnableDebugRuntime)
		{
			ReportLiveObjects();
		}
	}
	
	bool DeviceManagerDX12::CreateRenderTargets()
	{
		m_swapChainBuffers.resize(m_swapChainDesc.BufferCount);
		m_rhiSwapChainBuffers.resize(m_swapChainDesc.BufferCount);

		for (u32 i = 0; i < m_createInfo.SwapChainBufferCount; i++)
		{
			const HRESULT hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffers[i]));
			HR_RETURN(hr)

			nvrhi::TextureDesc textureDesc;
			textureDesc.width            = m_createInfo.BackBufferWidth;
			textureDesc.height           = m_createInfo.BackBufferHeight;
			textureDesc.sampleCount      = m_createInfo.SwapChainSampleCount;
			textureDesc.sampleQuality    = m_createInfo.SwapChainSampleQuality;
			textureDesc.format           = m_createInfo.SwapChainFormat;
			textureDesc.debugName        = "SwapChainBuffer"s + std::to_string(i);
			textureDesc.isRenderTarget   = true;
			textureDesc.isUAV            = false;
			textureDesc.initialState     = nvrhi::ResourceStates::Present;
			textureDesc.keepInitialState = true;

			m_rhiSwapChainBuffers[i] = m_nvrhiDevice->createHandleForNativeTexture(
				nvrhi::ObjectTypes::D3D12_Resource, nvrhi::Object(m_swapChainBuffers[i]), textureDesc);
		}

		return true;
	}
	
	void DeviceManagerDX12::ReleaseRenderTargets()
	{
		if (m_nvrhiDevice)
		{
			m_nvrhiDevice->waitForIdle();
			m_nvrhiDevice->runGarbageCollection();
		}
		
		for (auto e : m_frameFenceEvents)
		{
			::SetEvent(e);
		}

		m_rhiSwapChainBuffers.clear();
		m_swapChainBuffers.clear();
	}
}

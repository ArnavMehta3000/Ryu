#include "Graphics/Core/Device.h"
#include "Math/Math.h"
#include "Utils/StringConv.h"
#include "Profiling/Profiling.h"
#include <dxgidebug.h>

namespace Ryu::Gfx
{
	// --- Debug Layer ---
#if defined(RYU_BUILD_DEBUG)
	DWORD g_callbackCookie = 0;

	Device::DebugLayer::DebugLayer()
	{
		RYU_PROFILE_SCOPE();
		const GraphicsConfig& config = GraphicsConfig::Get();

		if (config.EnableDebugLayer)
		{
			ComPtr<ID3D12Debug6> d3dDebug;
			if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebug))))
			{
				d3dDebug->EnableDebugLayer();
				RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(GFXDevice), "DX12 Debug layer enabled");

				if (config.EnableGPUBasedValidation)
				{
					d3dDebug->SetEnableGPUBasedValidation(TRUE);
					RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(GFXDevice), "DX12 GPU based validation enabled");
				}
			}
			else
			{
				RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(GFXDevice), "DX12 Debug layer not available");
			}

			ComPtr<IDXGIDebug1> dxgiDebug;
			if (SUCCEEDED(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
			{
				dxgiDebug->EnableLeakTrackingForThread();
			}
			else
			{
				RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(GFXDevice), "DXGI Debug layer not available");
			}

			ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
			if (SUCCEEDED(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
			{
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, TRUE);
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, TRUE);

				DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
				{
					80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
				};
				DXGI_INFO_QUEUE_FILTER filter{};
				filter.DenyList.NumIDs = _countof(hide);
				filter.DenyList.pIDList = hide;
				dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
			}
			else
			{
				RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(GFXDevice), "DXGI Info Queue not available");
			}
		}
	}

	Device::DebugLayer::~DebugLayer()
	{
		RYU_PROFILE_SCOPE();

		ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		{
			DXCall(dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_DETAIL)));
		}

	}

	void Device::DebugLayer::SetupSeverityBreaks(ComPtr<DX12::Device>& device, bool enable)
	{
		RYU_PROFILE_SCOPE();

		ComPtr<ID3D12InfoQueue> d3dInfoQueue;
		if (SUCCEEDED(device.As(&d3dInfoQueue)))
		{
			DXCallEx(d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, enable), device.Get());
			DXCallEx(d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, enable), device.Get());
			DXCallEx(d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, enable), device.Get());

			ComPtr<ID3D12InfoQueue1> infoQueue1;
			if (SUCCEEDED(d3dInfoQueue.As(&infoQueue1)))
			{
				const auto MessageCallback = [](
					MAYBE_UNUSED D3D12_MESSAGE_CATEGORY category,
					MAYBE_UNUSED D3D12_MESSAGE_SEVERITY severity,
					MAYBE_UNUSED D3D12_MESSAGE_ID id,
					MAYBE_UNUSED LPCSTR description,
					MAYBE_UNUSED void* context)
					{
						if (severity == D3D12_MESSAGE_SEVERITY_INFO || severity == D3D12_MESSAGE_SEVERITY_MESSAGE)
						{
							RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(GFXDevice), "D3D12 Validation Layer: {}", description);
						}

						if (severity == D3D12_MESSAGE_SEVERITY_ERROR)
						{
							RYU_LOG_ERROR(RYU_LOG_USE_CATEGORY(GFXDevice), "D3D12 Validation Layer: {}", description);
						}

						if (severity == D3D12_MESSAGE_SEVERITY_WARNING)
						{
							RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(GFXDevice), "D3D12 Validation Layer: {}", description);
						}
					};

				if (enable)
				{
					DXCallEx(infoQueue1->RegisterMessageCallback(MessageCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, this, &g_callbackCookie), device.Get());
				}
				else
				{
					DXCallEx(infoQueue1->UnregisterMessageCallback(g_callbackCookie), device.Get());
				}

			}
		}
	}

	void Device::DebugLayer::ReportLiveDeviceObjectsAndReleaseDevice(ComPtr<DX12::Device>& device)
	{
		ComPtr<ID3D12DebugDevice2> debugDevice;
		if (SUCCEEDED(device.As(&debugDevice)))
		{
			device.Reset();  // Release the device so that when we report we have the lowest possible nummber of refs
			DXCall(debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL));
		}
	}
#endif
	// --- Debug Layer ---

	std::shared_ptr<Device> Device::Create()
	{
		auto device = std::shared_ptr<Device>(new Device());
		device->Initialize();
		return device;
	}

	void Device::Destroy(Device& device)
	{
		device.m_rtvHeap.reset();
		device.m_dsvHeap.reset();
		device.m_srvHeap.reset();
		device.m_uavHeap.reset();

		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			device.ProcessDeferredReleases(i);
		}

		device.m_cmdCtx.reset();

		device.ProcessDeferredReleases(0);

		device.m_factory.Reset();
#if defined(RYU_BUILD_DEBUG)
		device.m_debugLayer.SetupSeverityBreaks(device.m_device, false);
		device.m_debugLayer.ReportLiveDeviceObjectsAndReleaseDevice(device.m_device);
#else
		device.m_device.Reset();  // Manually release device
#endif
	}

	void Device::Initialize()
	{
		RYU_PROFILE_SCOPE();
		CreateDevice();
		
		m_cmdCtx = std::make_shared<CommandContext>(weak_from_this(), CmdListType::Direct);
		CreateDescriptorHeaps();

		RYU_LOG_DEBUG(RYU_LOG_USE_CATEGORY(GFXDevice),
			"DX12 Device created with max feature level: {}",
			Internal::FeatureLevelToString(m_featureSupport.MaxSupportedFeatureLevel()));
	}

	void Device::SetDeferredReleaseFlag()
	{
		m_deferredReleaseFlag[m_cmdCtx->GetFrameIndex()] = 1;
	}

	bool Device::CheckDeferredReleaseFlag(const u32 frameIndex) const
	{
		return m_deferredReleaseFlag[frameIndex] == 1;
	}

	void Device::ProcessDeferredReleases(const u32 frameIndex)
	{
		std::lock_guard lock(m_deferredReleaseMutex);

		m_deferredReleaseFlag[frameIndex] = 0;

		std::span<DX12::Object*> objects(m_deferredReleases[frameIndex]);
		if (!objects.empty())
		{
			for (DX12::Object* object : objects)
			{
				if (object)
				{
					object->Release();
				}
			}
			m_deferredReleases[frameIndex].clear();
		}
	}

	void Device::DeferReleaseObject(DX12::Object* object)
	{
		if (!object)
		{
			return;
		}

		const u32 frameIndex = m_cmdCtx->GetFrameIndex();
		std::lock_guard lock(m_deferredReleaseMutex);

		m_deferredReleases[frameIndex].push_back(object);
		SetDeferredReleaseFlag();
	}

	void Device::CreateDevice()
	{
		RYU_PROFILE_SCOPE();

		const auto& config = GraphicsConfig::Get();
		const bool useWarpDevice = config.UseWARP;

		// Debug layer should be enabled by now if needed

		u32 dxgiFactoryFlags = 0;

		// --- Enable debug layer ---
#if defined(RYU_BUILD_DEBUG)
		if (config.EnableDebugLayer)
		{
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif

		// --- Create DXGI factory---
		DXCall(::CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));

		// Temporary workaround because SetStablePowerState() is crashing
		D3D12EnableExperimentalFeatures(0, nullptr, nullptr, nullptr);

		// --- Create Device ---
		ComPtr<DXGI::Adapter> adapter;
		GetHardwareAdapter(m_factory.Get(), &adapter);
		DXCall(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

		// --- Create WARP Device if needed ---
		if (!m_device)
		{
			if (useWarpDevice)
			{
				RYU_LOG_DEBUG(RYU_LOG_USE_CATEGORY(GFXDevice), "WARP software adapter requested");
			}
			else
			{
				RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(GFXDevice), "Failed to find a hardware adapter.  Falling back to WARP");
			}

			DXCall(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));
			DXCall(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
		}

		// Enable security breaks
#if defined(RYU_BUILD_DEBUG)
		m_debugLayer.SetupSeverityBreaks(m_device, true);
#endif

		// --- Set debug name & cache capabilities ---
		DX12::SetObjectName(m_device.Get(), "Main Device");
		DXCallEx(m_featureSupport.Init(m_device.Get()), m_device.Get());
	}

	void Device::CreateDescriptorHeaps()
	{
		m_rtvHeap = std::make_shared<DescriptorHeap>(weak_from_this(), DescHeapType::RTV, DescHeapFlags::None, 512);
		m_dsvHeap = std::make_shared<DescriptorHeap>(weak_from_this(), DescHeapType::DSV, DescHeapFlags::None, 512);
		m_srvHeap = std::make_shared<DescriptorHeap>(weak_from_this(), DescHeapType::CBV_SRV_UAV, DescHeapFlags::ShaderVisible, 4096);
		m_uavHeap = std::make_shared<DescriptorHeap>(weak_from_this(), DescHeapType::CBV_SRV_UAV, DescHeapFlags::None, 512);

		DX12::SetObjectName(m_rtvHeap->GetHeap(), "RTV Descriptor Heap");
		DX12::SetObjectName(m_dsvHeap->GetHeap(), "DSV Descriptor Heap");
		DX12::SetObjectName(m_srvHeap->GetHeap(), "SRV Descriptor Heap");
		DX12::SetObjectName(m_uavHeap->GetHeap(), "UAV Descriptor Heap");
	}

	void Device::GetHardwareAdapter(DXGI::Factory* pFactory, DXGI::Adapter** ppAdapter) const
	{
		RYU_PROFILE_SCOPE();

		ComPtr<DXGI::Adapter> adapter;
		*ppAdapter = nullptr;

		u64 maxVRAM = 0;
		for (UINT adapterIndex = 0;
			SUCCEEDED(pFactory->EnumAdapterByGpuPreference(
				adapterIndex,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
				IID_PPV_ARGS(&adapter)));
			adapterIndex++)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);


			if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE || desc.DedicatedVideoMemory > maxVRAM)
			{
				continue;  // Don't select the Basic Render Driver adapter.
			}

			maxVRAM = desc.DedicatedVideoMemory;
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				const std::string description = Utils::ToNarrowStr(desc.Description);
				RYU_LOG_DEBUG(RYU_LOG_USE_CATEGORY(GFXDevice),
					"Using GPU: {} ({}) - {:.2f} GB", description, desc.VendorId, maxVRAM * Math::BytesToGigaBytes);
				break;
			}
		}

		*ppAdapter = adapter.Detach();
	}
}

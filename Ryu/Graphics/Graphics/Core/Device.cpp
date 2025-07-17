#include "Graphics/Core/Device.h"
#include "Graphics/Debug/DebugLayer.h"
#include "Graphics/GraphicsConfig.h"
#include "Math/Math.h"
#include "Utils/StringConv.h"
#include "Profiling/Profiling.h"
#include <ranges>

namespace Ryu::Gfx
{
	RYU_LOG_DECLARE_CATEGORY(GFXDevice);

	DevicePtr Device::Create()
	{
		auto device = DevicePtr(new Device());
		device->Initialize();
		return device;
	}

	void Device::Destroy(Device& device)
	{
		auto resetComPtr = [](auto& ptr) { ptr.Reset(); };

		device.m_rtvHeap.Destroy();
		device.m_cmdList.Reset();
		device.m_cmdQueue.Reset();
		std::ranges::for_each(device.m_cmdAllocators, resetComPtr);
		device.m_fence.Reset();
		device.m_factory.Reset();

		if (device.m_fenceEvent)
		{
			::CloseHandle(device.m_fenceEvent);
		}

#if defined(RYU_BUILD_DEBUG)
		DebugLayer::SetupSeverityBreaks(device.m_device, false);
		DebugLayer::SetStablePowerState(device.m_device, false);
		DebugLayer::ReportLiveDeviceObjectsAndReleaseDevice(device.m_device);
		DebugLayer::Shutdown();
#else
		device.m_device.Reset();  // Manually release device
#endif
	}

	void Device::Initialize()
	{
		RYU_PROFILE_SCOPE();

		DebugLayer::Initialize();

		CreateDevice();
		CreateDescriptorHeap();
		CreateCommandQueue();
		CreateCommandList();
		CreateSynchronization();

		RYU_LOG_DEBUG(LogGFXDevice, "DX12 Device created with max feature level: {}",
			Internal::FeatureLevelToString(m_featureSupport.MaxSupportedFeatureLevel()));
	}

	void Device::CreateDevice()
	{
		RYU_PROFILE_SCOPE();

		const auto& config = GraphicsConfig::Get();
		const bool useWarpDevice = config.UseWARP;

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
		::D3D12EnableExperimentalFeatures(0, nullptr, nullptr, nullptr);

		// --- Create Device ---
		ComPtr<DXGI::Adapter> adapter;
		GetHardwareAdapter(m_factory.Get(), &adapter);
		DXCall(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

		// --- Create WARP Device if needed ---
		if (!m_device)
		{
			if (useWarpDevice)
			{
				RYU_LOG_DEBUG(LogGFXDevice, "WARP software adapter requested");
			}
			else
			{
				RYU_LOG_WARN(LogGFXDevice, "Failed to find a hardware adapter.  Falling back to WARP");
			}

			DXCall(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));
			DXCall(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
		}

#if defined(RYU_BUILD_DEBUG)
		// Enable severity breaks and stable power state
		DebugLayer::SetupSeverityBreaks(m_device, true);
		DebugLayer::SetStablePowerState(m_device, true);
#endif

		// --- Set debug name & cache capabilities ---
		DX12::SetObjectName(m_device.Get(), "Main Device");
		DXCallEx(m_featureSupport.Init(m_device.Get()), m_device.Get());
	}

	void Device::CreateCommandQueue()
	{
		RYU_PROFILE_SCOPE();

		D3D12_COMMAND_QUEUE_DESC desc
		{
			.Type     = DX12::ToNative(CommandListType::Direct),
			.Priority = DX12::ToNative(CommandQueuePriority::Normal),
			.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0
		};

		DXCallEx(m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_cmdQueue)), m_device.Get());
		DX12::SetObjectName(m_cmdQueue.Get(), "Direct Command Queue");
	}

	void Device::CreateCommandList()
	{
		RYU_PROFILE_SCOPE();

		const D3D12_COMMAND_LIST_TYPE type = DX12::ToNative(CommandListType::Direct);

		// Create command allocators for each frame
		for (u32 i = 0; i < m_cmdAllocators.size(); i++)
		{ 
			DXCallEx(m_device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_cmdAllocators[i])), m_device.Get());
			DX12::SetObjectName(m_cmdAllocators[i].Get(), 
				std::format("Direct Command Allcator {}", i).c_str());
		}
		
		DXCallEx(m_device->CreateCommandList(0, type, m_cmdAllocators[0].Get(), nullptr, IID_PPV_ARGS(&m_cmdList)), m_device.Get());
		DX12::SetObjectName(m_cmdList.Get(), "Direct Graphics Command List");
		
		m_cmdList->Close();  // Command lists are created in recording state, close them
	}

	void Device::CreateSynchronization()
	{
		RYU_PROFILE_SCOPE();
		
		// FrameIndex will be 0 on first call to GetCurrentBackBufferIndex()

		// Create fence
		DXCallEx(m_device->CreateFence(m_frameIndex, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)), m_device.Get());
		DX12::SetObjectName(m_fence.Get(), "Frame Fence");
		
		// We know that this function is called before the swapchain is created
		// We also know that first call to GetCurrentBackBufferIndex() will return 0
		// So we increase the fence at 0 value by one
		m_fenceValues[m_frameIndex]++;

		m_fenceEvent = ::CreateEvent(nullptr, false, false, nullptr);
		if (!m_fenceEvent)
		{
			RYU_LOG_ERROR(LogGFXDevice, "Failed to create fence event");
			DXCallEx(HRESULT_FROM_WIN32(::GetLastError()), m_device.Get());
		}

		WaitForGPU();
	}

	void Device::CreateDescriptorHeap()
	{
		RYU_PROFILE_SCOPE();

		// RTV Heap
		m_rtvHeap.Initialize(weak_from_this(), DescriptorHeapType::RTV, DescriptorHeapFlags::None, FRAME_BUFFER_COUNT);
	}

	void Device::GetHardwareAdapter(DXGI::Factory* pFactory, DXGI::Adapter** ppAdapter) const
	{
		RYU_PROFILE_SCOPE();

		ComPtr<DXGI::Adapter> adapter;
		*ppAdapter = nullptr;

		for (UINT adapterIndex = 0;
			SUCCEEDED(pFactory->EnumAdapterByGpuPreference(
				adapterIndex,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
				IID_PPV_ARGS(&adapter)));
			adapterIndex++)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
			{
				continue;  // Don't select the Basic Render Driver adapter.
			}

			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				const std::string description = Utils::ToNarrowStr(desc.Description);
				RYU_LOG_DEBUG(LogGFXDevice,
					"Using GPU: {} ({}) - {:.2f} GB", description, desc.VendorId, desc.DedicatedVideoMemory * Math::BytesToGigaBytes);
				break;
			}
		}

		*ppAdapter = adapter.Detach();
	}
	
	void Device::WaitForGPU()
	{
		RYU_PROFILE_SCOPE();
		
		// Schedule a signal command in the queue
		DXCallEx(m_cmdQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]), m_device.Get());

		// Wait until the fence has been crossed
		DXCallEx(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent), m_device.Get());
		::WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

		// Increment the fence value for the current frame
		m_fenceValues[m_frameIndex]++;
	}
	
	void Device::MoveToNextFrame(const u32 frameIndex)
	{
		const u64 currentFenceValue = m_fenceValues[m_frameIndex];
		
		// Schedule a signal command in the queue
		DXCallEx(m_cmdQueue->Signal(m_fence.Get(), currentFenceValue), m_device.Get());

		// Update frame index
		m_frameIndex = frameIndex;

		// If the next frame is not ready to be rendered yet, wait until it is ready
		if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
		{
			DXCallEx(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent), m_device.Get());
			::WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
		}

		// Set the fence value for the next frame
		m_fenceValues[m_frameIndex] = currentFenceValue + 1;
	}
}

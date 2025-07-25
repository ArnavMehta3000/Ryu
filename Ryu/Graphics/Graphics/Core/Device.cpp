#include "Graphics/Core/Device.h"
#include "Graphics/Debug/DebugLayer.h"
#include "Graphics/GraphicsConfig.h"
#include "Math/Math.h"
#include "Utils/StringConv.h"
#include "Profiling/Profiling.h"

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
		device.m_factory.Reset();

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
}

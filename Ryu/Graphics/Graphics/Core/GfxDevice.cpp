#include "Graphics/Core/GfxDevice.h"
#include "Common/Assert.h"
#include "Graphics/Core/Debug/DebugLayer.h"
#include "Utils/StringConv.h"
#include "Math/Math.h"

namespace Ryu::Gfx
{
	GfxDevice::GfxDevice(const GfxDeviceConfig& config)
		: m_config(config)
	{
		u32 factoryFlags = 0;

#if defined(RYU_BUILD_DEBUG)
		if (m_config.EnableDebugLayer)
		{
			DebugLayer::Initialize();
			RYU_LOG_TRACE("Creating DX12 device with debug layer enabled");
			factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif

		CreateFactory(factoryFlags);
		SelectAdapter();
		CreateDevice();

#if defined(RYU_BUILD_DEBUG)
		if (m_config.EnableDebugLayer)
		{
			DebugLayer::SetupSeverityBreaks(m_device, true);

			if (m_config.SetStablePowerState)
			{
				DebugLayer::SetStablePowerState(m_device, true);
			}
		}
#endif

		RYU_LOG_INFO("GfxDevice created successfully");
	}

	GfxDevice::~GfxDevice()
	{
#if defined(RYU_BUILD_DEBUG)
		if (m_config.EnableDebugLayer)
		{
			DebugLayer::SetupSeverityBreaks(m_device, false);
			DebugLayer::Shutdown();
			DebugLayer::ReportLiveDeviceObjectsAndReleaseDevice(m_device);
		}
#endif
	}

	u32 GfxDevice::GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const
	{
		return m_device->GetDescriptorHandleIncrementSize(type);
	}

	void GfxDevice::CreateFactory(u32 flags)
	{
		DXCall(::CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_factory)));
	}

	void GfxDevice::SelectAdapter()
	{
		if (m_config.UseWarpDevice)
		{
			ComPtr<IDXGIAdapter> warpAdapter;
			DXCall(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
			DXCall(warpAdapter.As(&m_adapter));
			RYU_LOG_INFO("Using WARP software adapter");
			return;
		}

		// Select the best hardware adapter
		for (u32 adapterIndex = 0;
			SUCCEEDED(m_factory->EnumAdapterByGpuPreference(
				adapterIndex,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
				IID_PPV_ARGS(&m_adapter)));
			adapterIndex++)
		{
			DXGI_ADAPTER_DESC1 desc;
			m_adapter->GetDesc1(&desc);

			// Skip software adapters
			if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
			{
				continue;
			}

			// Test if adapter supports D3D12
			if (SUCCEEDED(::D3D12CreateDevice(
				m_adapter.Get(),
				m_config.MinFeatureLevel,
				_uuidof(ID3D12Device),
				nullptr)))
			{
				const std::string description = Utils::ToNarrowStr(desc.Description);
				RYU_LOG_INFO("Selected GPU: {} - {:.2f} GB",
					description,
					Math::BytesToGB(desc.DedicatedVideoMemory));
				break;
			}

			m_adapter.Reset();
		}

		RYU_ASSERT(m_adapter, "Failed to find a suitable GPU adapter!");
	}

	void GfxDevice::CreateDevice()
	{
		// Determine the best feature level
		constexpr std::array featureLevels =
		{
			D3D_FEATURE_LEVEL_12_2,
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
		};

		// Create device with minimum feature level first
		DXCall(::D3D12CreateDevice(
			m_adapter.Get(),
			m_config.MinFeatureLevel,
			IID_PPV_ARGS(&m_device)));

		// Check for highest supported feature level
		D3D12_FEATURE_DATA_FEATURE_LEVELS caps{};
		caps.pFeatureLevelsRequested = featureLevels.data();
		caps.NumFeatureLevels = static_cast<u32>(featureLevels.size());

		if (SUCCEEDED(m_device->CheckFeatureSupport(
			D3D12_FEATURE_FEATURE_LEVELS,
			&caps,
			sizeof(caps))))
		{
			// Recreate device with max supported feature level
			m_device.Reset();
			DXCall(::D3D12CreateDevice(
				m_adapter.Get(),
				caps.MaxSupportedFeatureLevel,
				IID_PPV_ARGS(&m_device)));

			m_featureLevel = caps.MaxSupportedFeatureLevel;
			RYU_LOG_INFO("Using D3D Feature Level: {}.{}",
				(m_featureLevel >> 12) & 0xF,
				(m_featureLevel >> 8) & 0xF);
		}
		else
		{
			m_featureLevel = m_config.MinFeatureLevel;
		}

		DX12::SetObjectName(m_device.Get(), "Main Device");
	}
}

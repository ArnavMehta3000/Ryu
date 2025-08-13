#include "Graphics/Core/Core.h"
#include "Graphics/Debug/DebugLayer.h"
#include "Graphics/Core/CommandContext2.h"
#include "Graphics/GraphicsConfig.h"
#include "Logging/Logger.h"
#include "Common/Assert.h"
#include "Math/Math.h"
#include "Profiling/Profiling.h"
#include "Utils/StringConv.h"

namespace Ryu::Gfx::Core
{
	RYU_LOG_DECLARE_CATEGORY(GFX);

#pragma region Variables
	ComPtr<DX12::Device>    g_device;
	ComPtr<DXGI::Factory>   g_factory;
	ComPtr<DXGI::SwapChain> g_swapChain;
	CD3DX12FeatureSupport   g_featureSupport;
	CommandContext2         g_cmdCtx;
#pragma endregion

#pragma region Accessors
	const CD3DX12FeatureSupport& GetFeatureSupport() { return g_featureSupport; }
	const ComPtr<DX12::Device>& GetDevice() { return g_device; }
	const ComPtr<DXGI::Factory>& GetFactory() { return g_factory; }
#pragma endregion

#pragma region Forward declarations
	void CreateDevice();
	void GetHardwareAdapter(DXGI::Adapter** ppAdapter);
	void CreateSwapChain(HWND window, DXGI_FORMAT backBufferFormat);
#pragma endregion


	void Render()
	{
		g_cmdCtx.BeginFrame();

		//auto& cmdList = g_cmdCtx.GetCommandList();

		g_cmdCtx.EndFrame();
	}


	void Init(HWND window, DXGI_FORMAT backBufferFormat)
	{
		RYU_PROFILE_SCOPE();
		DebugLayer::Initialize();

		CreateDevice();
		CreateSwapChain(window, backBufferFormat);

		g_cmdCtx.Create(CommandListType::Direct);
	}

	void Shutdown()
	{
		RYU_PROFILE_SCOPE();

		ComRelease(g_factory);

#if defined(RYU_BUILD_DEBUG)
		DebugLayer::SetupSeverityBreaks(g_device, false);
		DebugLayer::SetStablePowerState(g_device, false);
		DebugLayer::ReportLiveDeviceObjectsAndReleaseDevice(g_device);
		DebugLayer::Shutdown();
#else
		ComRelease(g_device);  // Manually release device
#endif
		ComRelease(g_swapChain);
		ComRelease(g_device);
	}


	void CreateDevice()
	{
		RYU_PROFILE_SCOPE();

		u32 dxgiFactoryFlags = 0;

		// --- Enable debug layer ---
#if defined(RYU_BUILD_DEBUG)
		if (Gfx::IsDebugLayerEnabled())
		{
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif

		// --- Create DXGI factory---
		DXCall(::CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&g_factory)));

		// Temporary workaround because SetStablePowerState() is crashing
		::D3D12EnableExperimentalFeatures(0, nullptr, nullptr, nullptr);

		// --- Create Adapter ---
		ComPtr<DXGI::Adapter> adapter;
		GetHardwareAdapter(&adapter);

		// --- Create Device ---
		DXCall(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_device)));

		// --- Create WARP Device if needed ---
		if (!g_device)
		{
			if (Gfx::ShouldUseWARPDevice())
			{
				RYU_LOG_DEBUG(LogGFX, "WARP software adapter requested");
			}
			else
			{
				RYU_LOG_WARN(LogGFX, "Failed to find a hardware adapter.  Falling back to WARP");
			}

			DXCall(g_factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));
			DXCall(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_device)));
		}

		// Enable severity breaks and stable power state
		RYU_DEBUG_OP(DebugLayer::SetupSeverityBreaks(g_device, true));
		RYU_DEBUG_OP(DebugLayer::SetStablePowerState(g_device, true));

		// --- Set debug name & cache capabilities ---
		DX12::SetObjectName(g_device.Get(), "Main Device");
		DXCallEx(g_featureSupport.Init(g_device.Get()), g_device.Get());
	}

	void GetHardwareAdapter(DXGI::Adapter** ppAdapter)
	{
		RYU_PROFILE_SCOPE();

		ComPtr<DXGI::Adapter> adapter;
		*ppAdapter = nullptr;

		for (UINT adapterIndex = 0;
			SUCCEEDED(g_factory->EnumAdapterByGpuPreference(
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
				RYU_LOG_DEBUG(LogGFX,
					"Using GPU: {} - {:.2f} GB", description, desc.DedicatedVideoMemory * Math::BytesToGigaBytes);
				break;
			}
		}

		*ppAdapter = adapter.Detach();
	}

	void CreateSwapChain(HWND window, DXGI_FORMAT backBufferFormat)
	{
		RYU_ASSERT(g_factory, "DXGI factory is not initialized.");

		// Not finished implementing
	}
}

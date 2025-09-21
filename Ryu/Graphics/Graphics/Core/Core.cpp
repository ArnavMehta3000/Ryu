#include "Graphics/Core/Core.h"
#include "Graphics/Core/Debug/DebugLayer.h"
#include "Graphics/Core/CommandContext.h"
#include "Graphics/Core/DescriptorAllocator.h"
#include "Graphics/Core/DescriptorHeap.h"
#include "Graphics/Core/SwapChain.h"
#include "Graphics/GraphicsConfig.h"
#include "Logging/Logger.h"
#include "Common/Assert.h"
#include "Math/Math.h"
#include "Profiling/Profiling.h"
#include "Utils/StringConv.h"

namespace Ryu::Gfx::Core
{
	using HeapAllocArray = std::array<DescriptorAllocator, (size_t)DescriptorHeapType::_COUNT>;

#pragma region Variables
	ComPtr<DX12::Device>    g_device;
	ComPtr<DXGI::Factory>   g_factory;
	SwapChain               g_swapChain;
	CD3DX12FeatureSupport   g_featureSupport;
	DescriptorHeap          g_shaderVisibleHeap;
	CommandContext          g_cmdCtx;
	HeapAllocArray          g_heapAllocators{ DescriptorAllocator(DescriptorHeapType::CBV_SRV_UAV), DescriptorAllocator(DescriptorHeapType::Sampler),
											  DescriptorAllocator(DescriptorHeapType::RTV)        , DescriptorAllocator(DescriptorHeapType::DSV) };
#pragma endregion

#pragma region Accessors
	const CD3DX12FeatureSupport& GetFeatureSupport() { return g_featureSupport; }
	const ComPtr<DX12::Device>& GetDevice() { return g_device; }
	const ComPtr<DXGI::Factory>& GetFactory() { return g_factory; }
	const SwapChain& GetSwapChain() { return g_swapChain; }
	CommandContext& GetCommandContext() { return g_cmdCtx; }
	DescriptorHeap& GetShaderVisibleHeap() { return g_shaderVisibleHeap; }
	DescriptorAllocator& GetDescriptorAllocator(DescriptorHeapType type) { return g_heapAllocators[(size_t)type]; }

#pragma endregion

#pragma region Forward declarations
	void CreateDevice();
	void CreateSwapChain(HWND window);
	void GetHardwareAdapter(DXGI::Adapter** ppAdapter);
#pragma endregion


	void Render()
	{
		g_cmdCtx.BeginFrame();

		//auto& cmdList = g_cmdCtx.GetCommandList();

		g_cmdCtx.EndFrame();

		g_swapChain.Present();
	}


	void Init(HWND window)
	{
		RYU_PROFILE_SCOPE();

#if defined(RYU_BUILD_DEBUG)
		DebugLayer::Initialize();
#endif

		CreateDevice();
		g_cmdCtx.Create(CommandListType::Direct);
		CreateSwapChain(window);
		RYU_CODE_BLOCK("Create resources")
		{
		}

		// Wait for the GPU to finish all commands
		g_cmdCtx.Flush();
	}

	void Shutdown()
	{
		RYU_PROFILE_SCOPE();
		g_cmdCtx.Flush();

		DescriptorAllocator::DestroyAll();
		ComRelease(g_factory);
		g_swapChain.Destroy();
		g_cmdCtx.Destroy();

#if defined(RYU_BUILD_DEBUG)
		DebugLayer::SetupSeverityBreaks(g_device, false);
		DebugLayer::SetStablePowerState(g_device, false);
		DebugLayer::ReportLiveDeviceObjectsAndReleaseDevice(g_device);
		DebugLayer::Shutdown();
#else
		ComRelease(g_device);  // Manually release device
#endif
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
			RYU_LOG_TRACE("Creating DX12 device with debug layer enabled");
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
				RYU_LOG_DEBUG("WARP software adapter requested");
			}
			else
			{
				RYU_LOG_WARN("Failed to find a hardware adapter. Falling back to WARP");
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

		RYU_LOG_DEBUG("Created DX12 device with feature level {}", Internal::FeatureLevelToString(g_featureSupport.MaxSupportedFeatureLevel()).data());
	}

	void CreateSwapChain(HWND window)
	{
		RYU_ASSERT(g_factory, "DXGI factory is not initialized.");

		g_swapChain.Create(window);
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

			if (SUCCEEDED(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				const std::string description = Utils::ToNarrowStr(desc.Description);
				RYU_LOG_DEBUG("Using GPU: {} - {:.2f} GB", description, Math::BytesToGB(desc.DedicatedVideoMemory));
				break;
			}
		}

		*ppAdapter = adapter.Detach();
	}

	void Resize(u32 width, u32 height)
	{
		RYU_PROFILE_SCOPE();

		if (width == 0 || height == 0)
		{
			return;
		}

		g_cmdCtx.Flush();
		g_swapChain.Resize();
	}
}

#include "DX12Core.h"
#include "Graphics/Config.h"
#include "Graphics/Internal/DXInternal.h"
#include "Graphics/DX12/DX12Command.h"
#include "Graphics/DX12/DX12Resources.h"
#include "Graphics/DX12/DX12Surface.h"
#include "Graphics/DX12/DX12Helpers.h"
#include "App/WindowBase.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics::DX12::Core
{
	namespace
	{
		RYU_LOG_CATEGORY(DX12Core);

		constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVEL{ D3D_FEATURE_LEVEL_11_0 };
		constexpr DXGI_FORMAT DEFAULT_RENDER_TARGET_FORMAT{ DXGI_FORMAT_R8G8B8A8_UNORM_SRGB };

		ID3D12Device8*                      g_device{ nullptr };
		DX12Command                         g_gfxCommand;
		std::unique_ptr<DX12Surface>        g_surface{ nullptr };

		DescriptorHeap                      g_rtvDescHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		DescriptorHeap                      g_dsvDescHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		DescriptorHeap                      g_srvDescHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		DescriptorHeap                      g_uavDescHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		std::array<std::vector<IUnknown*>, FRAME_BUFFER_COUNT> g_deferredReleases;
		std::array<u32, FRAME_BUFFER_COUNT>                    g_deferredReleaseFlags{};
		std::mutex                                             g_deferredReleaseMutex;

		bool InitializationFailed()
		{
			LOG_ERROR(RYU_USE_LOG_CATEGORY(DX12Core), "DX12 initialization failed");

			Shutdown();
			return false;
		}

		void __declspec(noinline) ProcessDeferredRelease(u32 frameIndex)
		{
			std::lock_guard lock{ g_deferredReleaseMutex };

			// Clear flag
			g_deferredReleaseFlags[frameIndex] = 0;

			g_rtvDescHeap.ProcessDeferredFree(frameIndex);
			g_dsvDescHeap.ProcessDeferredFree(frameIndex);
			g_srvDescHeap.ProcessDeferredFree(frameIndex);
			g_uavDescHeap.ProcessDeferredFree(frameIndex);

			std::vector<IUnknown*>& deferredReleases = g_deferredReleases[frameIndex];
			for (auto& ptr : deferredReleases)
			{
				Release(ptr);
			}
			deferredReleases.clear();
		}

	}	

	bool Init()
	{
		LOG_INFO(RYU_USE_LOG_CATEGORY(DX12Core), "Initializing DX12");

		if (g_device)
		{
			LOG_DEBUG(RYU_USE_LOG_CATEGORY(DX12Core), "DX12 already initialized");
			return InitializationFailed();
		}

		const GraphicsConfig& config = GraphicsConfig::Get();
		if (config.EnableDebugLayer)
		{
			LOG_DEBUG(RYU_USE_LOG_CATEGORY(DX12Core), "Enabling graphics debug layer");

			ComPtr<ID3D12Debug6> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();

				if (config.EnableGPUBasedValidation)
				{
					LOG_DEBUG(RYU_USE_LOG_CATEGORY(DX12Core), "Enabling GPU-based validation");
					debugController->SetEnableGPUBasedValidation(TRUE);
				}
			}
			else
			{
				LOG_WARN(RYU_USE_LOG_CATEGORY(DX12Core), "Failed to enable DX12 debug layer! Ensure Graphics Tools feature is installed");
			}
		}

		IDXGIAdapter4* const adapter = Graphics::Internal::GetMainAdapter();
		
		DEBUG_ASSERT(adapter, "Failed to get main adapter");
		if (!adapter) return InitializationFailed();

		//  Get max feature level
		const D3D_FEATURE_LEVEL maxFeatureLevel = Graphics::Internal::GetMaxFeatureLevel();
		if (maxFeatureLevel < MIN_FEATURE_LEVEL)
		{
			LOG_ERROR(RYU_USE_LOG_CATEGORY(DX12Core), "Failed to get max feature level");
		}

		if (maxFeatureLevel < MIN_FEATURE_LEVEL) return InitializationFailed();

		// Create device
		DXCall(D3D12CreateDevice(adapter, maxFeatureLevel, IID_PPV_ARGS(&g_device)));
		DEBUG_ASSERT(g_device, "Failed to create device");

		bool heapInit = true;
		heapInit &= g_rtvDescHeap.Init(512, false);
		heapInit &= g_dsvDescHeap.Init(512, false);
		heapInit &= g_srvDescHeap.Init(4096, true);
		heapInit &= g_uavDescHeap.Init(512, false);
		if (!heapInit) return InitializationFailed();

		new (&g_gfxCommand) DX12Command(g_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
		if (!g_gfxCommand.GetCommandQueue()) return InitializationFailed();

		if (config.EnableDebugLayer)
		{
			ComPtr<ID3D12InfoQueue> infoQueue;
			DXCall(g_device->QueryInterface(IID_PPV_ARGS(&infoQueue)));

			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		}

		DX12_NAME_OBJECT(g_device, L"Main Device");
		DX12_NAME_OBJECT(g_rtvDescHeap.GetHeap(), L"RTV Descriptor Heap");
		DX12_NAME_OBJECT(g_dsvDescHeap.GetHeap(), L"DSV Descriptor Heap");
		DX12_NAME_OBJECT(g_srvDescHeap.GetHeap(), L"SRV Descriptor Heap");
		DX12_NAME_OBJECT(g_uavDescHeap.GetHeap(), L"UAV Descriptor Heap");

		LOG_TRACE(RYU_USE_LOG_CATEGORY(DX12Core), "Initialized DX12");

		return true;
	}

	void Shutdown()
	{
		LOG_INFO(RYU_USE_LOG_CATEGORY(DX12Core), "Shutting down DX12");

		// Remove surface
		LOG_TRACE(RYU_USE_LOG_CATEGORY(DX12Core), "Removing surface");
		g_gfxCommand.Flush();
		g_surface.reset();

		g_gfxCommand.Release();

		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			ProcessDeferredRelease(i);
		}

		g_rtvDescHeap.Release();
		g_dsvDescHeap.Release();
		g_srvDescHeap.Release();
		g_uavDescHeap.Release();

		// Some types only use deferred release. Process them here
		ProcessDeferredRelease(0);

		if (const GraphicsConfig& config = GraphicsConfig::Get(); config.EnableDebugLayer)
		{
			{
				ComPtr<ID3D12InfoQueue> infoQueue;
				DXCall(g_device->QueryInterface(IID_PPV_ARGS(&infoQueue)));

				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
			}

			ComPtr<ID3D12DebugDevice> debugDevice;
			DXCall(g_device->QueryInterface(IID_PPV_ARGS(&debugDevice)));
			
			Release(g_device);

			DXCall(debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_SUMMARY | D3D12_RLDO_IGNORE_INTERNAL));
		}

		Release(g_device);

		LOG_TRACE(RYU_USE_LOG_CATEGORY(DX12Core), "Shutdown DX12");
	}

	ISurface* CreateSurface(App::WindowBase* window)
	{
		DEBUG_ASSERT(window && window->GetHWND(), "Invalid window handle");

		g_surface = std::make_unique<DX12Surface>(window);
		g_surface->CreateSwapChain(g_gfxCommand.GetCommandQueue(), DEFAULT_RENDER_TARGET_FORMAT);

		LOG_INFO(RYU_USE_LOG_CATEGORY(DX12Core), "Created DX12 surface");

		return g_surface.get();
	}

	void OnResizeSurface(u32 width, u32 height)
	{
		DEBUG_ASSERT(g_surface, "Surface not created");

		g_gfxCommand.Flush();
		g_surface->OnResize(width, height);
	}

	void RenderSurface()
	{
		DEBUG_ASSERT(g_surface, "Surface not created");

		g_gfxCommand.BeginFrame();

		// Record commands
		ID3D12GraphicsCommandList6* cmdList{ g_gfxCommand.GetCommandList() };

		const u32 frameIndex = GetCurrentFrameIndex();
		if (g_deferredReleaseFlags[frameIndex])
		{
			ProcessDeferredRelease(frameIndex);
		}

		g_surface->Present();

		g_gfxCommand.EndFrame();
	}

	void CreateRootSignature()
	{
		CD3DX12_ROOT_PARAMETER1 params[3];
		params[0].InitAsConstants(2, 0, 0, D3D12_SHADER_VISIBILITY_PIXEL);
		params[1].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
	}

	ID3D12Device8* const GetDevice()
	{
		return g_device;
	}

	DXGI_FORMAT GetDefaultRenderTargetFormat()
	{
		return DEFAULT_RENDER_TARGET_FORMAT;
	}

	u32 GetCurrentFrameIndex()
	{
		return g_gfxCommand.GetFrameIndex();
	}

	void SetDeferredReleaseFlag()
	{
		g_deferredReleaseFlags[GetCurrentFrameIndex()] = 1;
	}

	DescriptorHeap& GetRTVDescHeap() { return g_rtvDescHeap; }

	DescriptorHeap& GetDSVDescHeap() { return g_dsvDescHeap; }

	DescriptorHeap& GetSRVDescHeap() { return g_srvDescHeap; }

	DescriptorHeap& GetUAVDescHeap() { return g_uavDescHeap; }

	namespace Internal
	{
		void DeferredRelease(IUnknown* ptr)
		{
			const u32 frameIndex = GetCurrentFrameIndex();
			std::lock_guard lock{ g_deferredReleaseMutex };

			g_deferredReleases[frameIndex].push_back(ptr);
			SetDeferredReleaseFlag();
		}
	}
}

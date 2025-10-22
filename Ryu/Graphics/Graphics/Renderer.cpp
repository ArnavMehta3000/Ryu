#include "Renderer.h"
#include "Common/Assert.h"
#include "Globals/Globals.h"
#include "Utils/StringConv.h"
#include "Graphics/Core/Debug/DebugLayer.h"
#include "Math/Math.h"
#include <DirectXColors.h>

namespace Ryu::Gfx
{
	const bool g_enableDebugLayer = Globals::g_isDebug;
	const bool g_enableValidation = true;
	const bool g_useWarpDevice = false;
	const bool g_allowTearing = true;
	const bool g_isVsync = true;
	const DXGI_FORMAT g_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	Renderer::Renderer(HWND window)
		: m_hWnd(window)
	{
		// Get window size
		[](HWND hWNd, u32& outWidth, u32& outHeight) -> void
		{
			RECT rc{};
			::GetClientRect(hWNd, &rc);
			outWidth = static_cast<u32>(rc.right - rc.left);
			outHeight = static_cast<u32>(rc.bottom - rc.top);
		}(m_hWnd, m_width, m_height);

		// Create DXGI factory
		u32 dxgiFactoryFlags = 0;

#if defined(RYU_BUILD_DEBUG)
		DebugLayer::Initialize();
		if (g_enableDebugLayer)
		{
			RYU_LOG_TRACE("Creating DX12 device with debug layer enabled");
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif

		DXCall(::CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));

		CreateDevice();
		CreateCommandQueue();
		CreateSwapChain();
		CreateDescriptorHeaps();
		CreateFrameResources();
		CreateCmdList();
		CreateSynchronization();

		WaitForPreviousFrame();
	}

	Renderer::~Renderer()
	{
		WaitForPreviousFrame();

		if (m_fenceEvent)
		{
			::CloseHandle(m_fenceEvent);
		}

		ComRelease(m_factory);
		ComRelease(m_swapChain);

		for (auto& rt : m_renderTargets)
		{
			ComRelease(rt);
		}

		ComRelease(m_rtvHeap);
		ComRelease(m_cmdList);
		ComRelease(m_cmdQueue);
		ComRelease(m_cmdAllocator);
		ComRelease(m_fence);

		RYU_DEBUG_OP(DebugLayer::SetupSeverityBreaks(m_device, false));  // Stops hard crash when reporting live objects
		RYU_DEBUG_OP(DebugLayer::Shutdown());  // Reports DXGI
		RYU_DEBUG_OP(DebugLayer::ReportLiveDeviceObjectsAndReleaseDevice(m_device));
		
	}
	
	void Renderer::Render()
	{
		DXCall(m_cmdAllocator->Reset());
		DXCall(m_cmdList->Reset(m_cmdAllocator.Get(), nullptr));

		m_cmdList->RSSetViewports(1, &m_viewport);
		m_cmdList->RSSetScissorRects(1, &m_scissorRect);

		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_renderTargets[m_frameIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_cmdList->ResourceBarrier(1, &barrier);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_descriptorSize);
		m_cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		m_cmdList->ClearRenderTargetView(rtvHandle, DirectX::Colors::CornflowerBlue, 0, nullptr);

		barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_renderTargets[m_frameIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);
		m_cmdList->ResourceBarrier(1, &barrier);

		DXCall(m_cmdList->Close());

		// -------------------- Command list populated, now render

		ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
		m_cmdQueue->ExecuteCommandLists(1, cmdLists);

		// Not dowing tearing/VSYNC compatibility checks here
		DXCall(m_swapChain->Present(g_isVsync ? 1 : 0, 0));

		WaitForPreviousFrame();
	}
	
	void Renderer::OnResize(u32 w, u32 h)
	{
		RYU_LOG_TRACE("Renderer begin resize");

		m_width = w;
		m_height = h;

		WaitForPreviousFrame();
		
		// Release all references to swap chain buffers
		for (auto& rt : m_renderTargets)
		{
			ComRelease(rt);
		}

		DXGI_SWAP_CHAIN_DESC1 desc;
		m_swapChain->GetDesc1(&desc);

		DXCall(m_swapChain->ResizeBuffers(
			FRAME_BUFFER_COUNT,
			m_width,
			m_height,
			desc.Format,
			desc.Flags
		));

		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, (f32)m_width, (f32)m_height);
		m_scissorRect = CD3DX12_RECT(0, 0, m_width, m_height);

		CreateFrameResources();

		RYU_LOG_DEBUG("Renderer resized {}x{}", w, h);
	}
	
	void Renderer::CreateDevice()
	{
		if (g_useWarpDevice)
		{
			ComPtr<IDXGIAdapter> warpAdapter;
			DXCall(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

			DXCall(::D3D12CreateDevice(
				warpAdapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&m_device)
			));
		}
		else
		{
			ComPtr<DXGI::Adapter> adapter;
			for (u32 adapterIndex = 0;
				SUCCEEDED(m_factory->EnumAdapterByGpuPreference(
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
					RYU_LOG_INFO("Using GPU: {} - {:.2f} GB", description, Math::BytesToGB(desc.DedicatedVideoMemory));
					break;
				}
			}

			RYU_ASSERT(adapter, "Failed to find a suitable GPU adapter!");

			constexpr std::array featureLevels =
			{
				D3D_FEATURE_LEVEL_12_2,
				D3D_FEATURE_LEVEL_12_1,
				D3D_FEATURE_LEVEL_12_0,
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0
			};

			// Create device
			DXCall(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
			
			D3D12_FEATURE_DATA_FEATURE_LEVELS caps{};
			caps.pFeatureLevelsRequested = featureLevels.data();
			caps.NumFeatureLevels        = (u32)featureLevels.size();

			DXCall(m_device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &caps, sizeof(caps)));
			DXCall(::D3D12CreateDevice(adapter.Get(), caps.MaxSupportedFeatureLevel, IID_PPV_ARGS(&m_device)));
		
			RYU_DEBUG_OP(DebugLayer::SetupSeverityBreaks(m_device, true));
			//RYU_DEBUG_OP(DebugLayer::SetStablePowerState(m_device, true));

			DX12::SetObjectName(m_device.Get(), "Main Device");
		}
	}
	
	void Renderer::CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC desc
		{
			.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0
		};

		DXCall(m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_cmdQueue)));
		DX12::SetObjectName(m_cmdQueue.Get(), "Graphics Command Queue");

		// Create command allocator
		DXCall(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator)));
	}
	
	void Renderer::CreateSwapChain()
	{
		DXGI_SWAP_CHAIN_DESC1 scDesc{};
		scDesc.AlphaMode          = DXGI_ALPHA_MODE_IGNORE;
		scDesc.BufferCount        = FRAME_BUFFER_COUNT;
		scDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.Format             = g_backBufferFormat;
		scDesc.Width              = m_width;
		scDesc.Height             = m_height;
		scDesc.Scaling            = DXGI_SCALING_NONE;
		scDesc.Stereo             = FALSE;
		scDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scDesc.SampleDesc.Count   = 1;
		scDesc.SampleDesc.Quality = 0;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc
		{
			.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
			.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED,
			.Windowed         = TRUE
		};

		ComPtr<IDXGISwapChain1> swapChain1;
		DXCall(m_factory->CreateSwapChainForHwnd(
			m_cmdQueue.Get(),
			m_hWnd,
			&scDesc,
			&fsDesc,
			nullptr,
			&swapChain1));

		m_swapChain.Reset();
		DXCall(swapChain1.As(&m_swapChain));
		
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, (f32)m_width, (f32)m_height);
		m_scissorRect = CD3DX12_RECT(0, 0, m_width, m_height);
	}

	void Renderer::CreateDescriptorHeaps()
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc
		{
			.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = FRAME_BUFFER_COUNT,
			.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask       = 0
		};

		DXCall(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtvHeap)));
		DX12::SetObjectName(m_rtvHeap.Get(), "RTV Heap");

		m_descriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void Renderer::CreateFrameResources()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		static constexpr std::array objectNames = { "Backbuffer 0", "Backbuffer 1", "Backbuffer 2", "Backbuffer 3" };
		static_assert(FRAME_BUFFER_COUNT <= objectNames.size());  // We can have more names, but not less

		for (u32 i = 0; i < m_renderTargets.size(); i++)
		{
			DXCall(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));

			m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
			DX12::SetObjectName(m_renderTargets[i].Get(), objectNames[i]);

			rtvHandle.Offset(1, m_descriptorSize);
		}
	}
	
	void Renderer::CreateCmdList()
	{
		// PSO not created yet
		DXCall(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_cmdList)));
		DX12::SetObjectName(m_cmdList.Get(), "Graphics Command List");
		m_cmdList->Close();
	}
	
	void Renderer::CreateSynchronization()
	{
		DXCall(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		DX12::SetObjectName(m_fence.Get(), "Frame Fence");
		m_fenceValue = 1;

		m_fenceEvent = ::CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
	}
	
	void Renderer::WaitForPreviousFrame()
	{
		// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
		// This is code implemented as such for simplicity.

		// Signal and increment fence value
		const u64 fence = m_fenceValue;
		DXCall(m_cmdQueue->Signal(m_fence.Get(), fence));
		m_fenceValue++;

		// Wait until the previous frame is finished
		if (m_fence->GetCompletedValue() < fence)
		{
			DXCall(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
			::WaitForSingleObject(m_fenceEvent, INFINITE);
		}

		// Increment the frame index
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	}
}

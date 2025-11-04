#include "Graphics/Core/GfxDevice.h"
#include "Graphics/Core/GfxDeviceChild.h"
#include "Common/Assert.h"
#include "Graphics/GraphicsConfig.h"
#include "Graphics/Core/Debug/DebugLayer.h"
#include "Utils/StringConv.h"
#include "Math/Math.h"
#include <DirectXColors.h>

namespace Ryu::Gfx
{
	constexpr DXGI_FORMAT g_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	DeviceChild::DeviceChild(Device* device)
		: m_device(device)
	{
		m_device->AddDeviceChild(this);
		m_isRegistered = true;
	}

	DeviceChild::~DeviceChild()
	{
		if (m_isRegistered && m_device)
		{
			ReleaseObject();
			m_device->RemoveDeviceChild(this);
		}
	}

	// ---------------------------------------------------------------------

	Device::Device(HWND window)
		: m_window(window)
		, m_isDebugLayerEnabled(Config::IsDebugLayerEnabled())
		, m_isValidationEnabled(Config::IsValidationLayerEnabled())
		, m_isWarpDevice(Config::ShouldUseWarpDevice())
	{
		{
			auto [w, h] = GetClientSize();
			m_width = w;
			m_height = h;
		}

		// Create DXGI factory
		u32 dxgiFactoryFlags = 0;

#if defined(RYU_BUILD_DEBUG)
		DebugLayer::Initialize(m_isDebugLayerEnabled, m_isValidationEnabled);
		if (m_isDebugLayerEnabled)
		{
			RYU_LOG_TRACE("Creating DX12 device with debug layer enabled");
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif

		DXCall(::CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));

		CreateDevice();
	}

	Device::~Device()
	{
		ComRelease(m_factory);
		ComRelease(m_swapChain);

		for (DeviceChild* deviceChild : m_deviceChildren)
		{
			if (deviceChild)
			{
				deviceChild->ReleaseObject();
				deviceChild->DisconnectFromDevice();  // Prevent them from trying to unregister
			}
		}
		m_deviceChildren.clear();

		RYU_DEBUG_OP(DebugLayer::SetupSeverityBreaks(m_device, false));  // Stops hard crash when reporting live objects
		RYU_DEBUG_OP(DebugLayer::Shutdown());  // Reports DXGI
		RYU_DEBUG_OP(DebugLayer::ReportLiveDeviceObjectsAndReleaseDevice(m_device));
	}

	void Device::Initialize()
	{
		m_cmdQueue = std::make_unique<CommandQueue>(this, D3D12_COMMAND_LIST_TYPE_DIRECT, "Graphics Command Queue");
		m_cmdList  = std::make_unique<CommandList>(this, D3D12_COMMAND_LIST_TYPE_DIRECT, "Graphics Command List");
		m_rtvHeap  = std::make_unique<DescriptorHeap>(this, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, FRAME_BUFFER_COUNT, false, "Frame RTV Heap");
		m_dsvHeap  = std::make_unique<DescriptorHeap>(this, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false, "Frame DSV Heap");
		m_fence    = std::make_unique<Fence>(this, m_fenceValues[m_frameIndex], "Graphics Fence");

		m_fenceValues[m_frameIndex]++;

		CreateSwapChain();
		CreateFrameResources(false);
	}
	
	std::pair<u32, u32> Device::GetClientSize() const
	{
		RECT rc{};

		::GetClientRect(m_window, &rc);
		u32 width  = u32(rc.right - rc.left);
		u32 height = u32(rc.bottom - rc.top);

		return std::make_pair(width, height);
	}

	void Device::AddDeviceChild(DeviceChild* deviceChild)
	{
		m_deviceChildren.push_back(deviceChild);
	}

	void Device::RemoveDeviceChild(DeviceChild* deviceChild)
	{
		if (deviceChild)
		{
			m_deviceChildren.erase(
				std::remove(m_deviceChildren.begin(), m_deviceChildren.end(), deviceChild),
				m_deviceChildren.end());
		}
	}

	void Device::BeginFrame()
	{
		m_cmdList->Begin(m_frameIndex);
		m_cmdList->SetViewports(
			std::span<const CD3DX12_VIEWPORT>(&m_viewport, 1),
			std::span<const CD3DX12_RECT>(&m_scissorRect, 1)
		);
	}

	void Device::EndFrame()
	{
		m_cmdList->End();
		m_cmdQueue->ExecuteCommandList(*m_cmdList);

	}

	void Device::Present()
	{
		DXCall(m_swapChain->Present(Config::ShouldUseVsync() ? 1 : 0, 0));
		MoveToNextFrame();
	}
	
	void Device::WaitForGPU()
	{
		m_cmdQueue->Signal(*m_fence, m_fenceValues[m_frameIndex]);
		m_fence->Wait(m_fenceValues[m_frameIndex]);
		m_fenceValues[m_frameIndex]++;
	}

	void Device::MoveToNextFrame()
	{
		const u64 currentFenceValue = m_fenceValues[m_frameIndex];
		
		m_cmdQueue->Signal(*m_fence, currentFenceValue);
		m_fence->Wait(currentFenceValue);
		
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();		
		m_fenceValues[m_frameIndex] = currentFenceValue + 1;
	}

	void Device::ResizeBuffers(u32 w, u32 h)
	{
		if (w == 0 || h == 0 || (w == m_width && h == m_height))
		{
			return;
		}

		m_width  = w;
		m_height = h;

		WaitForGPU();

		// Release all references to swap chain buffers
		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			m_renderTargets[i]->ReleaseObject();
			m_fenceValues[i] = m_fenceValues[m_frameIndex];
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

		CreateFrameResources(true);

		RYU_LOG_DEBUG("Frame buffers resized {}x{}", w, h);
	}

	void Device::SetBackBufferRenderTarget(bool shouldClear)
	{
		// Assuming we have already transitioned the resource
		const DescriptorHandle rtvHandle = m_rtvHeap->GetHandle(m_frameIndex);
		m_cmdList->SetRenderTarget(rtvHandle, {});

		if (shouldClear)
		{
			m_cmdList->GetNative()->ClearRenderTargetView(rtvHandle.CPU, DirectX::Colors::DarkSlateGray, 0, nullptr);
		}
	}

	void Device::CreateDevice()
	{
		if (m_isWarpDevice)
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
			caps.NumFeatureLevels = u32(featureLevels.size());

			DXCall(m_device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &caps, sizeof(caps)));
			DXCall(::D3D12CreateDevice(adapter.Get(), caps.MaxSupportedFeatureLevel, IID_PPV_ARGS(&m_device)));

			RYU_DEBUG_OP(DebugLayer::SetupSeverityBreaks(m_device, true));
			//RYU_DEBUG_OP(DebugLayer::SetStablePowerState(m_device, true));

			DX12::SetObjectName(m_device.Get(), "Main Device");
		}
	}
	
	void Device::CreateSwapChain()
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
			m_cmdQueue->GetNative(),
			m_window,
			&scDesc,
			&fsDesc,
			nullptr,
			&swapChain1));

		m_swapChain.Reset();
		DXCall(swapChain1.As(&m_swapChain));

		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		m_viewport    = CD3DX12_VIEWPORT(0.0f, 0.0f, (f32)m_width, (f32)m_height);
		m_scissorRect = CD3DX12_RECT(0, 0, m_width, m_height);
	}
	
	void Device::CreateFrameResources(bool isResizing)
	{
		static constexpr std::array objectNames = { "Backbuffer 0", "Backbuffer 1", "Backbuffer 2", "Backbuffer 3" };
		static_assert(FRAME_BUFFER_COUNT <= objectNames.size());  // We can have more names, but not less

		for (u32 i = 0; i < m_renderTargets.size(); i++)
		{
			// Get existing handle if we are resizing, otherwise allocate
			DX12::Resource* resource = nullptr;
			const DescriptorHandle rtvHandle = isResizing ? m_rtvHeap->GetHandle(i) : m_rtvHeap->Allocate();
			
			DXCall(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&resource)));
			
			if (isResizing)
			{
				// Reuse existing Texture, just update its resource
				m_renderTargets[i]->UpdateTextureResource(resource, objectNames[i]);
			}
			else
			{
				m_renderTargets[i] = std::make_unique<Texture>(this, resource, objectNames[i]);
			}

			m_renderTargets[i]->CreateRenderTarget(nullptr, rtvHandle);
		}
	}
}

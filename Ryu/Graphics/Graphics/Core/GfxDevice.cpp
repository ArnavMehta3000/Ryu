#include "Graphics/Core/GfxDevice.h"
#include "Graphics/GraphicsConfig.h"
#include "Graphics/Core/Debug/DebugLayer.h"
#include "Utils/StringConv.h"
#include "Math/Math.h"

namespace Ryu::Gfx
{
	i32 g_vsync = 0;

	GfxDevice::GfxDevice(HWND window)
		: m_hWnd(window)
		, m_isFirstFrame(true)
	{
		g_vsync = GetSyncInterval();  // From graphics config

		// Get window size
		[](HWND hWNd, u32& outWidth, u32& outHeight) -> void
		{
			RECT rc{};
			::GetClientRect(hWNd, &rc);
			outWidth = static_cast<u32>(rc.right - rc.left);
			outHeight = static_cast<u32>(rc.bottom - rc.top);
		}(m_hWnd, m_width, m_height);

		u32 dxgiFactoryFlags = 0;

#if defined(RYU_BUILD_DEBUG)
		DebugLayer::Initialize();
		if (Gfx::IsDebugLayerEnabled())
		{
			RYU_LOG_TRACE("Creating DX12 device with debug layer enabled");
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif
		DXCall(::CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_dxgiFactory)));

		ComPtr<DXGI::Adapter> adapter;
		for (UINT adapterIndex = 0;
			SUCCEEDED(m_dxgiFactory->EnumAdapterByGpuPreference(
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
		caps.NumFeatureLevels = (u32)featureLevels.size();
		DXCall(m_device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &caps, sizeof(caps)));
		DXCall(::D3D12CreateDevice(adapter.Get(), caps.MaxSupportedFeatureLevel, IID_PPV_ARGS(&m_device)));

		DXCall(m_capabilities.Init(m_device.Get()));

		RYU_DEBUG_OP(DebugLayer::SetupSeverityBreaks(m_device, true));
		RYU_DEBUG_OP(DebugLayer::SetStablePowerState(m_device, true));

		DX12::SetObjectName(m_device.Get(), "Main Device");

		m_graphicsQueue.Create(this, CommandListType::Direct, "Graphics Queue");
		//m_computeQueue.Create(this, CommandListType::Compute, "Compute Queue");
		//m_copyQueue.Create(this, CommandListType::Copy, "Copy Queue");

		// Create command lists
		for (auto& cmdList : m_graphicsCmdLists)
		{
			cmdList = std::make_unique<GfxCommandList>(this, CommandListType::Direct, "Graphics Command List");
		}

		// Create descriptor heaps
		for (u32 i = 0; i < (u32)DescriptorHeapType::_COUNT; i++)
		{
			GfxDescriptorAllocator::Desc desc
			{
				.Type            = static_cast<DescriptorHeapType>(i),
				.DescriptorCount = 1024,
				.IsShaderVisible = false
			};

			m_descriptorAllocatorsCPU[i] = std::make_unique<GfxDescriptorAllocator>(this, desc);
		}
	}

	GfxDevice::~GfxDevice()
	{
		RYU_LOG_TRACE("Destroying GfxDevice");

		WaitForGPU();
		ProcessReleaseQueue();
		m_frameFence.Wait(m_frameFenceValues[m_swapChain->GetBackBufferIndex()]);

#if defined(RYU_BUILD_DEBUG)
		DebugLayer::SetupSeverityBreaks(m_device, false);
		DebugLayer::SetStablePowerState(m_device, false);
		//DebugLayer::ReportLiveDeviceObjectsAndReleaseDevice(m_device);
		DebugLayer::Shutdown();
#endif
	}

	GfxCommandQueue& GfxDevice::GetCommandQueue(CommandListType type)
	{
		using enum Ryu::Gfx::CommandListType;

		switch (type)
		{
		case Direct: return m_graphicsQueue;
		//case Compute: return m_computeQueue;
		//case Copy: return m_copyQueue;
		default:
		{
			RYU_LOG_WARN("Requested queue type ({}) is not supported! Returning graphics queue.", EnumToString(type));
			return m_graphicsQueue;
		}
		}
	}

	void GfxDevice::WaitForGPU()
	{
		m_graphicsQueue.Signal(m_waitFence, m_waitFenceValue);
		m_waitFence.Wait(m_waitFenceValue);
		m_waitFenceValue++;

		//m_computeQueue.Signal(m_waitFence, m_waitFenceValue);
		//m_waitFence.Wait(m_waitFenceValue);
		//m_waitFenceValue++;

		//copy_queue.Signal(m_waitFence, m_waitFenceValue);
		//m_waitFence.Wait(m_waitFenceValue);
		//m_waitFenceValue++;
	}

	void GfxDevice::BeginFrame()
	{
		u32 backBufferIndex = m_swapChain->GetBackBufferIndex();
		m_frameFence.Wait(m_frameFenceValues[backBufferIndex]);

		m_graphicsCmdLists[backBufferIndex]->Begin();
	}

	void GfxDevice::EndFrame()
	{
	}

	GfxDescriptor GfxDevice::AllocateDescriptorCPU(DescriptorHeapType type)
	{
		return m_descriptorAllocatorsCPU[(u64)type]->AllocateDescriptor();
	}

	void GfxDevice::FreeDescriptorCPU(GfxDescriptor descriptor, DescriptorHeapType type)
	{
		m_descriptorAllocatorsCPU[(u64)type]->FreeDescriptor(descriptor);
	}

	std::unique_ptr<GfxTexture> GfxDevice::CreateBackBufferTexture(const GfxTexture::Desc& desc, DX12::Resource* backbuffer)
	{
		return std::make_unique<GfxTexture>(this, desc, backbuffer);
	}
	
	void GfxDevice::ProcessReleaseQueue()
	{
	}
}


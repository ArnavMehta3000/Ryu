#include "Graphics/Device.h"
#include "Math/Math.h"
#include "Utils/StringConv.h"
#include <dxgidebug.h>
#include <libassert/assert.hpp>

namespace Ryu::Gfx
{
	Device::LiveObjectReporter::~LiveObjectReporter()
	{
		ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			ComPtr<IDXGIInfoQueue> infoQueue;
			DXCall(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&infoQueue)));

			infoQueue->ClearStoredMessages(DXGI_DEBUG_ALL);
			DXCall(debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_DETAIL)));

			if (infoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL) > 0)
			{
				RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(GraphicsDevice), "Live DXGI objects found!");
			}
		}
	}
	
	Device::Device() 
		: DeviceObject(this)
		, m_deleteQueue(this)
	{
		const auto& config = GraphicsConfig::Get();

		// --- Create DXGI factory---
		u32 factoryCreateFlags = 0;
		if (config.EnableDebugLayer)
		{
			factoryCreateFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

		DXCall(::CreateDXGIFactory2(factoryCreateFlags, IID_PPV_ARGS(&m_factory)));

		// --- Enable debug layer ---
		if (config.EnableDebugLayer)
		{
			ComPtr<ID3D12Debug6> debug;
			if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
			{
				debug->EnableDebugLayer();
				RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(GraphicsDevice), "DX12 Debug layer enabled");

				if (config.EnableGPUBasedValidation)
				{
					debug->SetEnableGPUBasedValidation(TRUE);
					RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(GraphicsDevice), "DX12 GPU based validation enabled");
				}
			}
		}

		// --- Enumerate adapters ---
		ComPtr<DXGI::Adapter> adapter;
		if (!config.UseWARP)
		{
			u32 adapterIndex = 0;
			DXGI_GPU_PREFERENCE gpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;

			while (m_factory->EnumAdapterByGpuPreference(adapterIndex++, gpuPreference, IID_PPV_ARGS(&adapter)) == S_OK)
			{
				DXGI_ADAPTER_DESC3 desc{};
				adapter->GetDesc3(&desc);
				const std::string description = Utils::ToNarrowStr(desc.Description);

				RYU_LOG_DEBUG(RYU_LOG_USE_CATEGORY(GraphicsDevice),
					"GPU: {} ({}) - {:.2f} GB", description, desc.VendorId, desc.DedicatedVideoMemory * Math::BytesToGigaBytes);

				u32 outputIndex = 0;
				ComPtr<DXGI::Output> output;
				while (adapter->EnumOutputs(outputIndex++, &output) == S_OK)
				{
					ComPtr<DXGI::Output6> output6;
					if (SUCCEEDED(output.As(&output6)))
					{
						DXGI_OUTPUT_DESC1 outputDesc{};
						output6->GetDesc1(&outputDesc);

						RYU_LOG_DEBUG(RYU_LOG_USE_CATEGORY(GraphicsDevice),
							"Monitor {} - {}x{} - HDR: {} - {} BPP - Min Lum {} - Max Lum {} - MaxFFL {}",
							outputIndex,
							outputDesc.DesktopCoordinates.right - outputDesc.DesktopCoordinates.left,
							outputDesc.DesktopCoordinates.bottom - outputDesc.DesktopCoordinates.top,
							outputDesc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 ? "Yes" : "No",
							outputDesc.BitsPerColor,
							outputDesc.MinLuminance,
							outputDesc.MaxLuminance,
							outputDesc.MaxFullFrameLuminance);
					}
				}
			}

			// Get best adapter again
			m_factory-> EnumAdapterByGpuPreference(0, gpuPreference, IID_PPV_ARGS(&adapter));
			DXGI_ADAPTER_DESC3 desc{};
			adapter->GetDesc3(&desc);
			RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(GraphicsDevice), "Using {}", Utils::ToNarrowStr(desc.Description));


			// --- Create Device ---
			ComPtr<DXGI::Adapter> adap;
			GetHardwareAdapter(m_factory.Get(), &adap);
			DXCall(::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)));
		}

		// --- Fallback to WARP ---
		if (!m_device)
		{
			RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(GraphicsDevice), "Failed to find valid adapter. Falling back to use WARP.");
			m_factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
			DXCall(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(&m_device)));
		}

		// --- Set debug name & cache capabilities ---
		DX12::SetObjectName(m_device.Get(), "Main Device");
		RYU_TODO("Cache device capabilities using CD3DXFeatureSupport");

		// --- Set break on severity ---
		ComPtr<ID3D12InfoQueue> infoQueue;
		if (SUCCEEDED(m_device.As(&infoQueue)))
		{
			DXCallEx(infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true), m_device.Get());
			DXCallEx(infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true), m_device.Get());
			DXCallEx(infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true), m_device.Get());
			RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(GraphicsDevice), "D3D Validation Break on Severity Enabled");

			ComPtr<ID3D12InfoQueue1> infoQueue1;
			if (infoQueue.As(&infoQueue1))
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
						RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(GraphicsDevice), "D3D12 Validation Layer: {}", description);
					}

					if (severity == D3D12_MESSAGE_SEVERITY_ERROR)
					{
						RYU_LOG_ERROR(RYU_LOG_USE_CATEGORY(GraphicsDevice), "D3D12 Validation Layer: {}", description);
					}

					if (severity == D3D12_MESSAGE_SEVERITY_WARNING)
					{
						RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(GraphicsDevice), "D3D12 Validation Layer: {}", description);
					}
				};

				DWORD callbackCookie = 0;
				DXCall(infoQueue1->RegisterMessageCallback(MessageCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, this, &callbackCookie));
			}
		}

		m_frameFence = Memory::CreateRef<Fence>(this, "Frame Fence");

		RYU_TODO("Set up command queues");
		RYU_TODO("Set up heaps");
	}
	
	Device::~Device()
	{
		IdleGPU();
		
		// Disable break on validation before destroying to not make live-leak detection break each time.
		ComPtr<ID3D12InfoQueue> infoQueue;
		if (SUCCEEDED(m_device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
		{
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, FALSE);
		}
	}
		
	void Device::IdleGPU()
	{
		RYU_TODO("Tick frame");
		m_frameFence->CPUWait(m_frameFence->GetLastSignaledValue());

		RYU_TODO("Wait for idle on command queue");
	}

	void Device::DeferReleaseObject(DX12::Object* object)
	{
		if (object)
		{
			m_deleteQueue.Enqueue(object, m_frameFence.Get());
		}
	}

	void Device::GetHardwareAdapter(IDXGIFactory7* pFactory, DXGI::Adapter** ppAdapter)
	{
		ComPtr<DXGI::Adapter> adapter;
		*ppAdapter = nullptr;

		for (UINT adapterIndex = 0;
			SUCCEEDED(pFactory->EnumAdapterByGpuPreference(
				adapterIndex,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
				IID_PPV_ARGS(&adapter)));
			adapterIndex++)
		{
			DXGI_ADAPTER_DESC3 desc;
			adapter->GetDesc3(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
			{
				continue;  // Don't select the Basic Render Driver adapter.
			}

			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
			{
				const std::string description = Utils::ToNarrowStr(desc.Description);
				RYU_LOG_DEBUG(RYU_LOG_USE_CATEGORY(GraphicsDevice),
					"Using GPU: {} ({}) - {:.2f} GB", description, desc.VendorId, desc.DedicatedVideoMemory * Math::BytesToGigaBytes);
				break;
			}
		}

		*ppAdapter = adapter.Detach();
	}
	
	Device::DeferredDeleteQueue::~DeferredDeleteQueue()
	{
		GetParent()->IdleGPU();
		Clean();
		ASSERT(m_deletionQueue.empty(), "Deferred delete queue is not empty!");
	}
	
	void Device::DeferredDeleteQueue::Enqueue(DX12::Object* resource, Fence* fence)
	{
		std::scoped_lock lock(m_queueMutex);
		FencedObject obj
		{
			.Fence      = fence,
			.FenceValue = fence->GetCurrentValue(),
			.Resource   = resource
		};

		m_deletionQueue.push(obj);
	}
	
	void Device::DeferredDeleteQueue::Clean()
	{
		std::scoped_lock lock(m_queueMutex);
		while (!m_deletionQueue.empty())
		{
			const FencedObject& obj = m_deletionQueue.front();
			if (!obj.Fence->IsComplete(obj.FenceValue))
			{
				break;
			}
			
			ASSERT(obj.Resource->Release() == 0);
			
			m_deletionQueue.pop();
		}
	}
}

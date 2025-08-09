#include "Graphics/Debug/DebugLayer.h"
#include "Graphics/Core/Device.h"
#include "Graphics/GraphicsConfig.h"
#include "Profiling/Profiling.h"
#include "Logger/Logger.h"
#include <dxgidebug.h>

namespace Ryu::Gfx::DebugLayer
{
	DWORD g_callbackCookie = 0;
	RYU_LOG_DECLARE_CATEGORY(GFXDebugLayer);

	void Initialize()
	{
		RYU_PROFILE_SCOPE();

		if (Gfx::IsDebugLayerEnabled())
		{
			ComPtr<ID3D12Debug6> d3dDebug;
			if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebug))))
			{
				d3dDebug->EnableDebugLayer();
				RYU_LOG_TRACE(LogGFXDebugLayer, "DX12 Debug layer enabled");

				if (Gfx::IsGPUBasedValidationEnabled())
				{
					d3dDebug->SetEnableGPUBasedValidation(TRUE);
					RYU_LOG_WARN(LogGFXDebugLayer, "DX12 GPU based validation enabled");
				}
			}
			else
			{
				RYU_LOG_WARN(LogGFXDebugLayer, "DX12 Debug layer not available");
			}

			ComPtr<IDXGIDebug1> dxgiDebug;
			if (SUCCEEDED(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
			{
				dxgiDebug->EnableLeakTrackingForThread();
			}
			else
			{
				RYU_LOG_WARN(LogGFXDebugLayer, "DXGI Debug layer not available");
			}

			ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
			if (SUCCEEDED(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
			{
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, TRUE);
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, TRUE);

				DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
				{
					80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
				};
				DXGI_INFO_QUEUE_FILTER filter{};
				filter.DenyList.NumIDs = _countof(hide);
				filter.DenyList.pIDList = hide;
				dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
			}
			else
			{
				RYU_LOG_WARN(LogGFXDebugLayer, "DXGI Info Queue not available");
			}
		}
	}

	void Shutdown()
	{
		RYU_PROFILE_SCOPE();

		ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		{
			DXCall(dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_DETAIL)));
		}
	}

	void SetupSeverityBreaks(ComPtr<DX12::Device>& device, bool enable)
	{
		RYU_PROFILE_SCOPE();

		ComPtr<ID3D12InfoQueue> d3dInfoQueue;
		if (SUCCEEDED(device.As(&d3dInfoQueue)))
		{
			DXCallEx(d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, enable), device.Get());
			DXCallEx(d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, enable), device.Get());
			DXCallEx(d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, enable), device.Get());

			ComPtr<ID3D12InfoQueue1> infoQueue1;
			if (SUCCEEDED(d3dInfoQueue.As(&infoQueue1)))
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
							RYU_LOG_INFO(LogGFXDebugLayer, "D3D12 Validation Layer: {}", description);
						}

						if (severity == D3D12_MESSAGE_SEVERITY_ERROR)
						{
							RYU_LOG_ERROR(LogGFXDebugLayer, "D3D12 Validation Layer: {}", description);
						}

						if (severity == D3D12_MESSAGE_SEVERITY_WARNING)
						{
							RYU_LOG_WARN(LogGFXDebugLayer, "D3D12 Validation Layer: {}", description);
						}
					};

				if (enable)
				{
					DXCallEx(infoQueue1->RegisterMessageCallback(MessageCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &g_callbackCookie), device.Get());
				}
				else
				{
					DXCallEx(infoQueue1->UnregisterMessageCallback(g_callbackCookie), device.Get());
				}

				// vvv Code below is from MiniEngine vvv

				// Suppress whole categories of messages
				//D3D12_MESSAGE_CATEGORY Categories[] = {};

				// Suppress messages based on their severity level
				D3D12_MESSAGE_SEVERITY Severities[] =
				{
					D3D12_MESSAGE_SEVERITY_INFO
				};

				// Suppress individual messages by their ID
				D3D12_MESSAGE_ID DenyIds[] =
				{
					// This occurs when there are uninitialized descriptors in a descriptor table, even when a
					// shader does not access the missing descriptors.  I find this is common when switching
					// shader permutations and not wanting to change much code to reorder resources.
					D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,

					// Triggered when a shader does not export all color components of a render target, such as
					// when only writing RGB to an R10G10B10A2 buffer, ignoring alpha.
					D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_PS_OUTPUT_RT_OUTPUT_MISMATCH,

					// This occurs when a descriptor table is unbound even when a shader does not access the missing
					// descriptors.  This is common with a root signature shared between disparate shaders that
					// don't all need the same types of resources.
					D3D12_MESSAGE_ID_COMMAND_LIST_DESCRIPTOR_TABLE_NOT_SET,

					// RESOURCE_BARRIER_DUPLICATE_SUBRESOURCE_TRANSITIONS
					D3D12_MESSAGE_ID_RESOURCE_BARRIER_DUPLICATE_SUBRESOURCE_TRANSITIONS,

					// Suppress errors from calling ResolveQueryData with timestamps that weren't requested on a given frame.
					D3D12_MESSAGE_ID_RESOLVE_QUERY_INVALID_QUERY_STATE,

					// Ignoring InitialState D3D12_RESOURCE_STATE_COPY_DEST. Buffers are effectively created in state D3D12_RESOURCE_STATE_COMMON.
					D3D12_MESSAGE_ID_CREATERESOURCE_STATE_IGNORED,
				};

				D3D12_INFO_QUEUE_FILTER NewFilter{};
				//NewFilter.DenyList.NumCategories = _countof(Categories);
				//NewFilter.DenyList.pCategoryList = Categories;
				NewFilter.DenyList.NumSeverities   = _countof(Severities);
				NewFilter.DenyList.pSeverityList   = Severities;
				NewFilter.DenyList.NumIDs          = _countof(DenyIds);
				NewFilter.DenyList.pIDList         = DenyIds;

				//infoQueue1->PushStorageFilter(&NewFilter);

				// ^^^ Code above is from MiniEngine ^^^
				// Storage filter suppression is not enabled until renderer is stable
				RYU_TODO("Enable storage filter for DX12 validation layer");
			}
		}
	}

	void ReportLiveDeviceObjectsAndReleaseDevice(ComPtr<DX12::Device>& device)
	{
		ComPtr<ID3D12DebugDevice2> debugDevice;
		if (SUCCEEDED(device.As(&debugDevice)))
		{
			ComRelease(device);  // Release the device so that when we report we have the lowest possible nummber of refs
			DXCall(debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL));
		}
	}

	void SetStablePowerState(ComPtr<DX12::Device>& device, bool enable)
	{
		// Look in the Windows Registry to determine if Developer Mode is enabled
		HKEY hKey;
		LSTATUS result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock", 0, KEY_READ, &hKey);

		bool devModeEnabled = true;
		if (result == ERROR_SUCCESS)
		{
			DWORD keyValue, keySize = sizeof(DWORD);
			result = ::RegQueryValueEx(hKey, L"AllowDevelopmentWithoutDevLicense", 0, NULL, (byte*)&keyValue, &keySize);

			if (result == ERROR_SUCCESS && keyValue == 1)
			{
				devModeEnabled = true;
			}
			::RegCloseKey(hKey);
		}

		if (devModeEnabled)
		{
			device->SetStablePowerState(enable);
		}
		else
		{
			RYU_LOG_WARN(LogGFXDebugLayer, "Enable Developer Mode on Windows 10 to get consistent profiling results");
		}
	}
}

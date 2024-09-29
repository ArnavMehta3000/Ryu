#include "DX12Device.h"
#include <Graphics/DXGI/DXGIAdapter.h>
#include <Graphics/Debug/DX12DebugDevice.h>

namespace Ryu::Graphics
{
	CreateResult<DX12Device::InterfaceType*> DX12Device::Create(const DXGIAdapter& adapter)
	{
		RYU_GFX_ASSERT(adapter, "Trying to create DX12Device with invalid DXGIAdapter");

		ComPtr<DX12Device::InterfaceType> outDevice;
		HRESULT hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&outDevice));
		if (SUCCEEDED(hr))
		{
#if defined(RYU_BUILD_DEBUG)
			ComPtr<ID3D12InfoQueue> infoQueue;
			if (SUCCEEDED(outDevice.As(&infoQueue)))
			{
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
			}
#endif
			outDevice->SetName(L"DX12Device");
			RYU_GFX_DEBUG("Created DX12Device");
			
			return outDevice.Detach();
		}

		return std::unexpected(hr);
	}

	DX12Device::DX12Device(InterfaceType* ptr)
		: ComPtr(ptr)
	{
	}

	DX12Device::~DX12Device()
	{
		Release();
	}

	DX12DebugDevice DX12Device::GetDebugDevice() const
	{
		return DX12DebugDevice(Get());
	}

	void DX12Device::Release()
	{
		if (Get())
		{
#if defined(RYU_BUILD_DEBUG)
			{  // Revert breakpoint changes
				ComPtr<ID3D12InfoQueue> infoQueue;
				if (SUCCEEDED(As(&infoQueue)))
				{
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, FALSE);
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, FALSE);
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, FALSE);
				}
			}

			DX12DebugDevice dbg(Get());
			Reset();
			dbg->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
#endif
			Reset();
		}
	}
}


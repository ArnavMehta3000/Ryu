#include "DX12Device.h"
#include <Graphics/DXGI/DXGIAdapter.h>
#include <Graphics/Debug/DX12DebugDevice.h>

namespace Ryu::Graphics
{
	DX12Device::DX12Device(DXGIAdapter adapter)
	{
		if (adapter == nullptr)
		{
			RYU_GFX_WARN("Trying to create DX12Device with nullptr DXGIAdapter");
			return;
		}

		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(ReleaseAndGetAddressOf()))))
		{
#if defined(RYU_BUILD_DEBUG)
			ComPtr<ID3D12InfoQueue> infoQueue;
			if (SUCCEEDED(As(&infoQueue)))
			{
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
			}
#endif

			Get()->SetName(L"DX12Device");
		}
	}

	DX12Device::~DX12Device()
	{
		if (Get())
		{
			Release();
		}
	}

	DX12DebugDevice DX12Device::GetDebugDevice() const
	{
		return DX12DebugDevice(Get());
	}

	void DX12Device::Release()
	{
#if defined(RYU_BUILD_DEBUG)
		{
			ComPtr<ID3D12InfoQueue> infoQueue;
			if (SUCCEEDED(As(&infoQueue)))
			{
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, FALSE);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, FALSE);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, FALSE);
			}
		}

		DX12DebugDevice dbg = GetDebugDevice();
		Reset();
		dbg->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
#endif

		Reset();
	}
}


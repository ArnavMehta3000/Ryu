#pragma once
#include "Graphics/Core/RHI.h"

namespace Ryu::Gfx::DebugLayer
{
	void Initialize();
	void Shutdown();

	void SetupSeverityBreaks(ComPtr<DX12::Device>& device, bool enable);
	void ReportLiveDeviceObjectsAndReleaseDevice(ComPtr<DX12::Device>& device);
	void SetStablePowerState(ComPtr<DX12::Device>& device, bool enable);
}

#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx::DebugLayer
{
	void Initialize(bool isDebugLayerEnabled, bool isValidationEnabled);
	void Shutdown();

	void SetupSeverityBreaks(ComPtr<DX12::Device>& device, bool enable);
	void ReportLiveDeviceObjectsAndReleaseDevice(ComPtr<DX12::Device>& device);
	void SetStablePowerState(ComPtr<DX12::Device>& device, bool enable);
}

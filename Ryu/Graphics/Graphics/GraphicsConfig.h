#pragma once

namespace Ryu::Gfx
{
	bool IsDebugLayerEnabled();
	bool IsGPUBasedValidationEnabled();
	bool IsTearingAllowed();
	bool ShouldUseWARPDevice();
	i32 GetSyncInterval();


	// Functions here are implemented in Renderer.cpp
}

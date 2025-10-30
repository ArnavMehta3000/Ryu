#pragma once

namespace Ryu::Gfx
{
	class Config
	{
	public:
		static bool IsDebugLayerEnabled();
		static bool IsValidationLayerEnabled();
		static bool ShouldUseWarpDevice();
		static bool ShouldAllowTearing();
		static bool ShouldUseVsync();
	};
}

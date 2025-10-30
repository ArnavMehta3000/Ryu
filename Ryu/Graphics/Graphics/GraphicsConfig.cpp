#include "Graphics/GraphicsConfig.h"
#include "Config/CVar.h"
#include "Globals/Globals.h"

namespace Ryu::Gfx
{
	using namespace ::Ryu::Config;

	static CVar<bool> s_enableDebugLayer(
		"Gfx.DebugLayer",
		Globals::g_isDebug,
		"Enable graphics debug layer");

	static CVar<bool> s_enableValidation(
		"Gfx.Validation",
		true,
		"Enable graphics validation layer");

	static CVar<bool> s_useWarpDevice(
		"Gfx.WarpDevice",
		false,
		"Use the graphics warp device");

	static CVar<bool> s_allowTearing(
		"Gfx.AllowTearing",
		true,
		"Allow screen tearing");

	static CVar<bool> s_isVsync(
		"Gfx.Vsync",
		true,
		"Enable vsync");
	
	bool Config::IsDebugLayerEnabled()      { return s_enableDebugLayer.Get(); }
	bool Config::IsValidationLayerEnabled() { return s_enableValidation.Get(); }
	bool Config::ShouldUseWarpDevice()      { return s_useWarpDevice.Get();    }
	bool Config::ShouldAllowTearing()       { return s_allowTearing.Get();     }
	bool Config::ShouldUseVsync()           { return s_isVsync.Get();          }
}

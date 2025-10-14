#include "Globals/Globals.h"
#include "Config/CmdLine.h"
#include "Renderer.h"

namespace Ryu::Gfx
{
	static Config::CVar<bool> cv_debugLayerEnabled(
		"Gfx.EnableDebugLayer",
		Globals::g_isDebug,
		"Enable graphics debug layer. Enabled by default in debug builds",
		Config::CVarFlags::Debug | Config::CVarFlags::ReadOnly);

	static Config::CVar<bool> cv_enableValidation(
		"Gfx.EnableValidation",
		false,
		"Enable GPU based validation. Off by default",
		Config::CVarFlags::Debug | Config::CVarFlags::ReadOnly);

	static Config::CVar<i32> cv_syncInterval(
		"Gfx.SyncInterval",
		0,
		"Swapchain sync interval. 0: no sync (default) | 1: vsync enabled");

	static Config::CVar<bool> cv_allowTearing(
		"Gfx.AllowTearing",
		true,
		"Allow screen tearing if supported. Note vsync and tearing cannot be enabled at the same time");

	static Config::CVar<bool> cv_useWarp(
		"Gfx.UseWARP",
		false,
		"Use WARP graphics device (Default: false)");


	bool IsDebugLayerEnabled()         { return cv_debugLayerEnabled.Get(); }
	bool IsGPUBasedValidationEnabled() { return cv_enableValidation.Get();  }
	bool IsTearingAllowed()            { return cv_allowTearing.Get();      }
	i32 GetSyncInterval()              { return cv_syncInterval.Get();      }
	bool ShouldUseWARPDevice()         { return cv_useWarp.Get();           }

	Renderer::Renderer(HWND window)
		: m_device(window)
	{
	}
	
	void Renderer::Render()
	{
		m_device.BeginFrame();
		m_device.EndFrame();
	}
	
	void Renderer::OnResize(u32 w, u32 h)
	{
	}
}

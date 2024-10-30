#include "SamplePlugin.h"


namespace Sample
{
	std::string_view SamplePlugin::GetDescription() const noexcept
	{
		return "Showcases plugin functionality";
	}

	std::string_view SamplePlugin::GetName() const noexcept
	{
		return "Sample Plugin";
	}

	u32 SamplePlugin::GetVersion() const noexcept
	{
		return 100;
	}

	bool SamplePlugin::OnActivate()
	{
		return true;
	}

	void SamplePlugin::OnDeactivate()
	{
		
	}

}

extern "C"
{
	using namespace Ryu::Plugin;

	_declspec(dllexport) PluginInterface* GetPluginInterface()
	{
		auto plugin = std::make_unique<Sample::SamplePlugin>();
		return plugin.release();
	}
}

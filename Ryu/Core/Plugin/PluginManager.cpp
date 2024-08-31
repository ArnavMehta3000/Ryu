#include "PluginManager.h"

namespace Ryu
{
	void PluginManager::AddPlugin(const std::string& name, const std::string& path)
	{
		m_plugins[name] = PluginEntry{ .PluginPath = path };
	}
}

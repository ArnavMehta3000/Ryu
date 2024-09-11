#include "PluginManager.h"

namespace Ryu
{
	void PluginManager::AddPlugin(const std::string& name, const PluginEntry& entry)
	{
		m_plugins.insert_or_assign(name, entry);
	}

	bool PluginManager::HasPlugin(const std::string& name) const 
	{
		return m_plugins.find(name) != m_plugins.end();
	}

	const PluginEntry* PluginManager::GetPluginEntry(const std::string& name) const
	{
		return HasPlugin(name) ? &m_plugins.at(name) : nullptr;
	}

	IPlugin* PluginManager::GetPlugin(const std::string& name)
	{
		const PluginEntry* entry = GetPluginEntry(name);
		return entry ? entry->Plugin : nullptr;
	}
}

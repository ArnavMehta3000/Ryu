#include "Plugin/PluginManager.h"

namespace Ryu::Plugin
{
	PluginManager::~PluginManager()
	{
		for (auto& [name, plugin] : m_loadedPlugins)
		{
			plugin.ShutdownFunc();
			plugin.FunctionTable.reset();
			UnloadDLL(plugin.Handle);
		}
	}
	
	void PluginManager::UnLoadPluginInterface(const std::string& apiName)
	{
		auto it = m_loadedPlugins.find(apiName);
		if (it != m_loadedPlugins.end())
		{
			it->second.ShutdownFunc();
			UnloadDLL(it->second.Handle);
			m_loadedPlugins.erase(it);
		}
	}
	
	HMODULE PluginManager::LoadDLL(const std::string& pluginPath)
	{
		return ::LoadLibraryA(pluginPath.c_str());
	}
	
	void PluginManager::UnloadDLL(HMODULE handle)
	{
		::FreeLibrary(handle);
	}
}

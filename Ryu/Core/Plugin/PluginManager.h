#pragma once
#include <Core/Plugin/PluginAPI.h>
#include <Core/Utils/DllLoader.h>
#include <unordered_map>
#include <memory>

namespace Ryu
{
	struct PluginEntry
	{
		std::string PluginPath;
		IPlugin* Plugin{ nullptr };
		DllLoader DLL;
		DestroyPlugin_f DestroyPluginFunc{ nullptr };
	};

	class RYU_API PluginManager
	{
	public:
		using PluginMap = std::unordered_map<std::string, PluginEntry>;

		RYU_DISABLE_COPY_AND_MOVE(PluginManager);
		PluginManager() = default;
		~PluginManager() = default;

		void AddPlugin(const std::string& name, const PluginEntry& entry);
		NODISCARD PluginMap& GetPluginsMap() { return m_plugins; }
		NODISCARD bool HasPlugin(const std::string& name) const;
		NODISCARD const PluginEntry* GetPluginEntry(const std::string& pluginName) const;
		
		NODISCARD IPlugin* GetPlugin(const std::string& pluginName);

		template <typename T>
		NODISCARD T* GetPlugin(const std::string& pluginName)
		{
			IPlugin* plugin = GetPlugin(pluginName);
			return plugin ? dynamic_cast<T*>(plugin) : nullptr;
		}
	private:
		PluginMap m_plugins;
	};
}
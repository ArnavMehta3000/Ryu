#pragma once
#include "Common/Common.h"
#include "Plugin/Plugin.h"
#include "Logger/Logger.h"
#include <vector>

namespace std::filesystem { class path; }

namespace Ryu::Plugin
{
	class PluginInterface;
	
	class PluginLoader
	{
		RYU_LOG_CATEGORY(PluginLoader);
		RYU_DISABLE_COPY_AND_MOVE(PluginLoader);

		using PluginLoadCallback = PluginInterface*(*)();

	public:
		PluginLoader();
		~PluginLoader() = default;

		void AddSearchDirectory(const std::filesystem::path& path);

		NODISCARD GenericPlugin Load(const std::filesystem::path& pluginPath, bool activate = true);

		template<typename T> requires IsComplete<StaticPluginProvider<T>>
		NODISCARD Plugin<T> Load(bool activate = true)
		{
			// This function is called if a static plugic provided is implemented
			std::unique_ptr<T> pluginInterface = StaticPluginProvider<T>::Instantiate();
			if (activate && !pluginInterface->Activate())
			{
				LOG_ERROR(RYU_USE_LOG_CATEGORY(PluginLoader), "Failed to load and activate plugin");
			}
			// Return one with empty DLL
			return Plugin<T>({}, std::move(pluginInterface));
		}

		template<typename T> requires (!IsComplete<StaticPluginProvider<T>>)
		NODISCARD Plugin<T> Load(bool activate)
		{
			GenericPlugin plugin = Load(T::Filename(), activate);
			return std::move(plugin).As<T>();
		}

	private:
		std::vector<std::filesystem::path> m_directories;
	};
}

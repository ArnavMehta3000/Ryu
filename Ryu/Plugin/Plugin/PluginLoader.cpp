#include "PluginLoader.h"
#include "Plugin/PluginInterface.h"
#include <algorithm>
#include <stdexcept>

namespace Ryu::Plugin
{
	namespace fs = std::filesystem;

	PluginLoader::PluginLoader()
	{
		AddSearchDirectory(".");
		AddSearchDirectory("./Plugins");
	}

	void PluginLoader::AddSearchDirectory(const std::filesystem::path& path)
	{
		fs::path absolutePath = fs::absolute(path);
		if (std::find(m_directories.begin(), m_directories.end(), absolutePath) != m_directories.end())
		{
			m_directories.push_back(absolutePath);
			LOG_TRACE(RYU_USE_LOG_CATEGORY(PluginLoader), "Added plugin search directory: {}", absolutePath.string());
		}
	}

	NODISCARD GenericPlugin PluginLoader::Load(const std::filesystem::path& pluginPath, bool activate)
	{
		fs::path path = pluginPath;

		// Add extension if not present
		if (path.extension() != ".dll")
		{
			path += ".dll";
		}

		if (!path.is_absolute())
		{
			for (const fs::path& dir : m_directories)
			{
				fs::path testPath = dir / path;
				if (fs::exists(testPath))
				{
					// We found the plugin
					path = testPath;
					break;
				}
			}
		}

		DLL dll;
		if (!dll.Load(path))
		{
			LOG_FATAL(RYU_USE_LOG_CATEGORY(PluginLoader), "Failed to load plugin. File not found: {}", path.string());
		}

		PluginLoadCallback loadFunc = reinterpret_cast<PluginLoadCallback>(dll.GetSymbol("GetPluginInterface"));
		if (!loadFunc)
		{
			LOG_FATAL(RYU_USE_LOG_CATEGORY(PluginLoader), "Failed to load plugin. Missing symbol: {}", path.string());
		}

		PluginInterface* pluginInterface = loadFunc();
		if (!pluginInterface)
		{
			LOG_FATAL(RYU_USE_LOG_CATEGORY(PluginLoader), "Failed to load plugin. Missing plugin interface: {}", path.string());
		}

		if (activate && !pluginInterface->Activate())
		{
			LOG_ERROR(RYU_USE_LOG_CATEGORY(PluginLoader), "Failed to activate plugin: {}", path.string());
		}

		return GenericPlugin(std::move(dll), std::unique_ptr<PluginInterface>(pluginInterface));
	}

}

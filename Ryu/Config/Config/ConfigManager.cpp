#include "ConfigManager.h"
#include "Config/ConfigBase.h"

namespace Ryu::Config
{
	void ConfigManager::Initialize(const fs::path& configDir)
	{
		m_configDir = configDir;
		if (!fs::exists(m_configDir))
		{
			fs::create_directories(m_configDir);
		}
	}

	void ConfigManager::RegisterConfig(ConfigBase* config)
	{
		m_registeredConfigs.push_back(config);
	}

	void ConfigManager::UnregisterConfig(ConfigBase* config)
	{
		auto it = std::find(m_registeredConfigs.begin(), m_registeredConfigs.end(), config);
		if (it != m_registeredConfigs.end())
		{
			m_registeredConfigs.erase(it);
		}
	}

	void ConfigManager::SaveAll()
	{
		for (auto* config : m_registeredConfigs)
		{
			if (config->IsDirty())
			{
				config->Save();
			}			
		}
	}
}

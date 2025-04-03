#include "ConfigManager.h"
#include "Config/ConfigBase.h"
#include "Profiling/Profiling.h"
#include <Logger/Logger.h>

namespace Ryu::Config
{
	RYU_LOG_CATEGORY(ConfigManager);

	void ConfigManager::Initialize(const fs::path& configDir)
	{
		RYU_PROFILE_SCOPE();
		
		const std::string dbgMsg = "Initialize ConfigManager: " + configDir.string();
		RYU_PROFILE_SCOPE_TEXT(dbgMsg);

		m_configDir = configDir;
		if (!fs::exists(m_configDir))
		{
			fs::create_directories(m_configDir);
		}
		
		OnInitialized.Emit(true);
	}

	void ConfigManager::RegisterConfig(ConfigBase* config)
	{
		if (m_configDir.empty())
		{
			LOG_ERROR(RYU_USE_LOG_CATEGORY(ConfigManager), "ConfigManager has not been initialized yet!");
			return;
		}
		
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
		RYU_PROFILE_SCOPE();
		RYU_PROFILE_BOOKMARK("Save all configs");

		for (auto* config : m_registeredConfigs)
		{
			if (config->IsDirty())
			{
				config->Save();
			}			
		}
	}
}

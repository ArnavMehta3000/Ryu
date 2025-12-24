#include "ConfigManager.h"
#include "Core/Config/ConfigBase.h"
#include "Core/Profiling/Profiling.h"
#include "Core/Logging/Logger.h"

namespace Ryu::Config
{

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
			RYU_LOG_ERROR("ConfigManager has not been initialized yet!");
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

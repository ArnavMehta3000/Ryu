#include "ConfigBase.h"
#include "Config/ConfigManager.h"
#include "Logger/Logger.h"

namespace Ryu::Config
{
	ConfigBase::~ConfigBase()
	{
		if (m_isRegistered)
		{
			ConfigManager::Get().UnregisterConfig(this);
		}

		if (m_isDirty)
		{
			LOG_WARN(RYU_USE_LOG_CATEGORY(ConfigManager), "Config was marked dirty before exiting. Changes will not be saved!");
		}
	}
	
	bool ConfigBase::Load()
	{
		try
		{
			fs::path path = GetFullPath();
			if (!std::filesystem::exists(path))
			{
				// File doesn't exist yet, create with default values
				LOG_TRACE(RYU_USE_LOG_CATEGORY(ConfigManager), "Creating new config file: {}", path.string());
				Serialize(m_table);
				m_isDirty = true;
				return Save();
			}

			LOG_INFO(RYU_USE_LOG_CATEGORY(ConfigManager), "Loading config file: {}", path.string());
			m_table = toml::parse_file(path.string());

			// Deserialize values
			Deserialize(m_table);

			m_isDirty = false;
			return true;
		}
		catch (const std::exception& e)
		{
			LOG_ERROR(RYU_USE_LOG_CATEGORY(ConfigManager), "Failed to load config: {}", e.what());
			return false;
		}
	}
	
	bool ConfigBase::Save() const
	{
		try
		{
			fs::path path = GetFullPath();

			// Make sure the directory exists
			fs::create_directories(path.parent_path());

			// Create a new TOML table for the current config values
			toml::table table;
			Serialize(table);

			// Save to file
			std::ofstream file(path);
			file << table;

			const_cast<ConfigBase*>(this)->m_isDirty = false;

			LOG_INFO(RYU_USE_LOG_CATEGORY(ConfigManager), "Saved config file: {}", path.string());
			return true;
		}
		catch (const std::exception& e)
		{
			LOG_ERROR(RYU_USE_LOG_CATEGORY(ConfigManager), "Error saving config file: {}", e.what());
			return false;
		}
	}
	
	fs::path ConfigBase::GetFullPath() const
	{
		return ConfigManager::Get().GetConfigDir() / m_filename;
	}
}

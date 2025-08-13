#include "ConfigBase.h"
#include "Profiling/Profiling.h"
#include "Config/ConfigManager.h"

namespace Ryu::Config
{
	toml::table& ConfigValueBase::GetOrCreateSectionTable(toml::table& table) const
	{
		if (!m_section.empty())
		{
			if (!table.contains(m_section))
			{
				table.insert(m_section, toml::table{});
			}
			return *table[m_section].as_table();
		}
		return table;
	}

	const toml::table* ConfigValueBase::GetSectionTable(const toml::table& table) const
	{
		if (!m_section.empty())
		{
			if (auto sectionNode = table.get(m_section))
			{
				return sectionNode->as_table();
			}
			return nullptr;
		}
		return &table;
	}

	void ConfigBase::SerializeAll(toml::table& table) const
	{
		RYU_PROFILE_SCOPE();
		for (auto& value : m_configValues)
		{
			value->Serialize(table);
		}
	}
	
	void ConfigBase::DeserializeAll(const toml::table& table)
	{
		RYU_PROFILE_SCOPE();
		for (auto& value : m_configValues)
		{
			value->Deserialize(table);
		}
	}

	ConfigBase::~ConfigBase()
	{
		if (m_isRegistered)
		{
			ConfigManager::Get().UnregisterConfig(this);
		}

		if (m_isDirty)
		{
			RYU_LOG_WARN("Config ({}) was marked dirty before exiting. Changes will not be saved!", m_filename);
		}
	}
	
	bool ConfigBase::Load()
	{
		RYU_PROFILE_SCOPE();
		try
		{
			const fs::path path = GetFullPath();
			const std::string dbgMsg = "Load config: " + path.string();
			RYU_PROFILE_SCOPE_TEXT(dbgMsg);
			
			if (!std::filesystem::exists(path))
			{
				// File doesn't exist yet, create with default values
				RYU_LOG_TRACE("Creating new config file: {}", path.string());
				Serialize(m_table);
				m_isDirty = true;
				return Save();
			}

			RYU_LOG_INFO("Loading config file: {}", path.string());
			m_table = toml::parse_file(path.string());

			// Deserialize values
			Deserialize(m_table);

			// Create temporary table with all current values and compare
			toml::table currentTable;
			Serialize(currentTable);
			if (currentTable != m_table)
			{
				// Tables differ - merge new fields into m_table
				m_table = currentTable;
				m_isDirty = true;
			}

			return true;
		}
		catch (const std::exception& e)
		{
			RYU_LOG_ERROR("Failed to load config: {}", e.what());
			return false;
		}
	}
	
	bool ConfigBase::Save() const
	{
		RYU_PROFILE_SCOPE();
		try
		{
			const fs::path path = GetFullPath();
			const std::string dbgMsg = "Save config: " + path.string();
			RYU_PROFILE_SCOPE_TEXT(dbgMsg);

			// Make sure the directory exists
			fs::create_directories(path.parent_path());

			// Create a new TOML table for the current config values
			toml::table table;
			Serialize(table);

			toml::format_flags flags = toml::format_flags::indentation | toml::format_flags::allow_unicode_strings;
			std::stringstream ss;
			ss << toml::toml_formatter{ table, flags };

			// Save to file
			std::ofstream file(path);
			file << ss.str();

			const_cast<ConfigBase*>(this)->m_isDirty = false;

			RYU_LOG_INFO("Saved config file: {}", path.string());
			return true;
		}
		catch (const std::exception& e)
		{
			RYU_LOG_ERROR("Error saving config file: {}", e.what());
			return false;
		}
	}
	
	fs::path ConfigBase::GetFullPath() const
	{
		return ConfigManager::Get().GetConfigDir() / m_filename;
	}
}

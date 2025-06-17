#include "App/Utils/PathManager.h"
#include <toml++/toml.hpp>

namespace Ryu::App
{
	PathManager::PathManager()
	{
		m_rootDir = FindRootDirectory();
		LoadConfig();
	}

	fs::path PathManager::FindRootDirectory()
	{
		// Start from current path and look for Ryu.toml marker file that identifies root Ryu directory
		fs::path currentPath = fs::current_path();
		while (!fs::exists(currentPath / "Ryu.toml")
			&& currentPath.has_parent_path())
		{
			currentPath = currentPath.parent_path();
		}
		return currentPath;
	}

	void PathManager::LoadConfig()
	{
		auto config = toml::parse_file((m_rootDir / "Ryu.toml").string());
		auto projectDir = config["Paths"]["ProjectDir"].value_or(std::string());

		m_projectDir  = m_rootDir / projectDir;
		m_gameDLLName = config["Paths"]["GameDLLName"].value_or(std::string());
		m_gameDLLPath = fs::current_path() / m_gameDLLName;
	}
}

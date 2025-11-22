#include "App/Utils/PathManager.h"
#include "Config/CVar.h"
#include <toml++/toml.hpp>

namespace Ryu::App
{
	static Config::CVar<std::string> s_projectRootDir(
		"App.ProjectRootDir",
		std::filesystem::current_path().string(),
		"Project root directory",
		Config::CVarFlags::ReadOnly);


	PathManager::PathManager()
	{
		m_projectDir = s_projectRootDir.Get();
		m_configDir = m_projectDir / "Config";
	}
}

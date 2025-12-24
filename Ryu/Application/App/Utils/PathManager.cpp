#include "Application/App/Utils/PathManager.h"
#include "Core/Config/CVar.h"
#include <toml++/toml.hpp>

namespace Ryu::App
{
	static Config::CVar<std::string> cv_projectDir(
		"App.ProjectDir",
		std::filesystem::current_path().string(),
		"Project root directory",
		Config::CVarFlags::ReadOnly);


	PathManager::PathManager()
	{
		m_projectDir = cv_projectDir.Get();
		m_configDir = m_projectDir / "Config";
	}
}

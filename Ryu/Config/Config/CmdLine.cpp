#include "Config/CmdLine.h"

namespace Ryu::Config
{
	ICVarBase* CmdLine::FindCVar(std::string_view name) const
	{
		if (name.empty())
		{
			return nullptr;
		}
		
		auto it = m_cvars.find(name.data());
		if (it == m_cvars.end())
		{
			return nullptr;
		}
		
		return it->second.get();
	}
	
	std::string CmdLine::GetCVarValue(std::string_view name) const
	{
		if (ICVarBase* cvar = FindCVar(name))
		{
			return cvar->GetAsString();
		}

		return "";
	}
	
	bool CmdLine::SetCVarValue(std::string_view name, std::string_view value)
	{
		if (ICVarBase* cvar = FindCVar(name))
		{
			return cvar->SetFromString(value);
		}

		return false;
	}

	bool CmdLine::ParseCommandLine()
	{
		EnsureCLIApp();
		
		try
		{
			std::wstring cmd(GetCommandLine());
			m_cliApp->parse(cmd, true);
		}
		catch (const CLI::ParseError& e)
		{
			OutputDebugStringA(e.what());
			m_cliApp->exit(e);
		}
		
		return true;
	}

	void CmdLine::SaveFileConfig(const fs::path& filename)
	{
		EnsureCLIApp();

		// Save to file
		std::ofstream file(filename);
		file << m_cliApp->config_to_str(true, true);
	}
	
	void CmdLine::EnsureCLIApp()
	{
		if (!m_cliApp) 
		{
			m_cliApp = std::make_unique<CLI::App>("Ryu Config", "Ryu");

			// Configure settings
			m_cliApp->allow_config_extras(true);
			
			m_cliApp->set_config(
				"--cfgfile",
				"RyuConfig.toml",
				"Read a config TOML file",
				false   // Required
			);
		}
	}
}

#include "Core/Config/CmdLine.h"

#if defined (RYU_BUILD_DEBUG)
	#include "Core/Utils/StringConv.h"
#endif

#include <sstream>
#include <print>

namespace Ryu::Config
{
	static CVar<bool> s_saveConfig(
		"Cfg.SaveFile",
		false,
		"Save config to file",
		CVarFlags::ReadOnly);

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
		
		return it->second;
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
			RYU_DEBUG_BLOCK(
				const std::string str = Utils::ToNarrowStr(cmd);
				::OutputDebugStringA(str.c_str());
				::OutputDebugStringA("\n");
				std::println("{}\n\n", str);
			)
			m_cliApp->parse(cmd, true);
		}
		catch (const CLI::ParseError& e)
		{
			// I'll manually dump the error to the debug console and stdout
			const auto Print = [](const std::string& msg)
			{
				::OutputDebugStringA(msg.c_str());
				::OutputDebugStringA("\n");

				std::println("{}", msg);
			};

			// Manually print error
			std::ostringstream ssOut, ssErr;
			m_cliApp->exit(e, ssOut, ssErr);
			
			const std::string out = ssOut.str();
			const std::string err = ssErr.str();
			
			if (!out.empty())
			{
				Print(out);
			}

			if (!err.empty())
			{
				Print(err);
			}

			return false;
		}
		
		return true;
	}

	void CmdLine::SaveFileConfig(const fs::path& filename)
	{
		EnsureCLIApp();

		try
		{
			// Save to file
			std::ofstream file(filename);
			file << m_cliApp->config_to_str(true, true);
		}
		catch (const std::exception& e)
		{
			OutputDebugStringA(e.what());
		}
	}
	
	void CmdLine::EnsureCLIApp()
	{
		if (!m_cliApp) 
		{
			m_cliApp = std::make_unique<CLI::App>("Ryu Config", "Ryu");

			// Configure settings
			m_cliApp->allow_config_extras(true);
			
			m_cliApp->set_config(
				"--Cfg.File",
				"RyuConfig.toml",
				"Read a config TOML file",
				false   // Required
			);
		}
	}
}

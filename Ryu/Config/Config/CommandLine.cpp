#include "CommandLine.h"
#include "Utils/StringConv.h"
#include <Windows.h>
#include <shellapi.h>
#include <charconv>

namespace Ryu::Config
{
	CommandLine::CommandLine(std::wstring_view cmdLine)
	{
		i32 argc = 0;
		LPWSTR* argv = ::CommandLineToArgvW(cmdLine.data(), &argc);

		if (argv)
		{
			for (int i = 0; i < argc; ++i)
			{
				std::wstring warg(argv[i]);
				m_arguments.emplace_back(Utils::ToNarrowStr(warg));
			}
			::LocalFree(argv);
		}

		ParseArguments();
	}

	bool CommandLine::GetBool(std::string_view key) const
	{
		auto it = m_argumentsMap.find(std::string(key));
		return it != m_argumentsMap.end() && std::holds_alternative<bool>(it->second);
	}

	std::optional<i32> CommandLine::GetInt(std::string_view key) const
	{
		auto it = m_argumentsMap.find(std::string(key));
		if (it != m_argumentsMap.end() && std::holds_alternative<i32>(it->second))
		{
			return std::get<i32>(it->second);
		}

		return std::nullopt;
	}

	std::optional<std::string> CommandLine::GetString(std::string_view key) const
	{
		auto it = m_argumentsMap.find(std::string(key));
		if (it != m_argumentsMap.end() && std::holds_alternative<std::string>(it->second))
		{
			return std::get<std::string>(it->second);
		}

		return std::nullopt;
	}

	void CommandLine::ParseArguments()
	{
		for (const auto& arg : m_arguments)
		{
			// Check if the argument contains an '=' (e.g., "-resolution=1920x1080")
			size_t equalsPos = arg.find('=');
			if (equalsPos != std::string::npos)
			{
				std::string flag = arg.substr(0, equalsPos);
				std::string valueStr = arg.substr(equalsPos + 1);

				// Try to parse the value as an integer
				int intValue;
				if (auto [ptr, ec] = std::from_chars(valueStr.data(), valueStr.data() + valueStr.size(), intValue); ec == std::errc())
				{
					m_argumentsMap[flag] = intValue;
				}
				else
				{
					// Otherwise, store it as a string
					m_argumentsMap[flag] = valueStr;
				}
			}
			else if (arg.starts_with('-'))
			{
				// Treat it as a boolean flag (e.g., "-fullscreen")
				m_argumentsMap[arg] = true;
			}
		}
	}
}

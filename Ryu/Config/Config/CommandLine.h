#pragma once
#include "Common/Common.h"
#include <variant>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

namespace Ryu::Config
{
	class CommandLine
	{
	public:
		using ArgumentValue = std::variant<bool, i32, std::string>;
		
		CommandLine() = default;
		explicit CommandLine(std::wstring_view cmdLine);

		bool GetBool(std::string_view key) const;
		std::optional<i32> GetInt(std::string_view key) const;
		std::optional<std::string> GetString(std::string_view key) const;

	private:
		void ParseArguments();

	private:
		std::vector<std::string> m_arguments;
		std::unordered_map<std::string, ArgumentValue> m_argumentsMap;
	};
}

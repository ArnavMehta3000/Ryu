#include "Helpers.h"
#include <algorithm>
#include <regex>

namespace Ryu::Scripting
{

	std::optional<std::string> GetFriendlyNameFromMetadata(const std::vector<std::string>& metadata)
	{
		static const std::regex friendlyNameRegex(R"(FriendlyName\s*=\s*(.+))");

		for (const auto& tag : metadata)
		{
			std::smatch match;
			if (std::regex_search(tag, match, friendlyNameRegex))
			{
				// Trim whitespace from the captured group
				std::string friendlyName = match[1].str();

				// Remove leading and trailing whitespace
				friendlyName.erase(0, friendlyName.find_first_not_of(" \t\n\r\f\v"));
				friendlyName.erase(friendlyName.find_last_not_of(" \t\n\r\f\v") + 1);

				return friendlyName;
			}
		}

		// Tag not found return std::nullopt
		return std::nullopt;
	}

	std::optional<std::string> ExtractFactoryFunctionName(const std::vector<std::string>& metadata)
	{
		// Regex to match Factory = FunctionName
		std::regex factoryRegex(R"(Factory\s*=\s*(.+))");

		for (const auto& tag : metadata)
		{
			std::smatch match;
			if (std::regex_search(tag, match, factoryRegex))
			{
				// Trim whitespace from the captured group
				std::string factoryName = match[1].str();

				// Remove leading and trailing whitespace
				factoryName.erase(0, factoryName.find_first_not_of(" \t\n\r\f\v"));
				factoryName.erase(factoryName.find_last_not_of(" \t\n\r\f\v") + 1);

				return factoryName;
			}
		}

		// Return empty optional if no factory name found
		return std::nullopt;
	}

}

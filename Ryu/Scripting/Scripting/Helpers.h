#pragma once
#include "Common/ObjectMacros.h"
#include <string>
#include <vector>
#include <optional>

namespace Ryu::Scripting
{
	/**
	 * @brief Returns the friendly name from the metadata
	 * @details Friendly name is given only if metadata tag [FriendlyName = CustomName] is present
	 * @param metadata Input metadata of type/method
	 * @return The friendly name if found. Else std::nullopt
	 */
	NODISCARD std::optional<std::string> GetFriendlyNameFromMetadata(const std::vector<std::string>& metadata);

	/**
	 * @brief Extracts the factory function name from the metadata
	 * @details Factory function name is given only if metadata tag [Factory = FunctionName] is present
	 * @param metadata Input metadata of type/method
	 * @return The factory function name if found. Else std::nullopt
	 */
	NODISCARD std::optional<std::string> ExtractFactoryFunctionName(const std::vector<std::string>& metadata);
}

#pragma once
#include <string_view>
#include "Common/ObjectMacros.h"
#include "Common/API.h"

namespace Ryu::Logging
{
	/**
	 * @brief Represents a log category
	 * @details Wraps a string to be used as a log category
	 */
	struct LogCategory
	{
		std::string_view Name;

		explicit constexpr LogCategory(std::string_view name) : Name(name) {}
	};
}

#define RYU_LOG_DECLARE_CATEGORY(CategoryName) static constexpr ::Ryu::Logging::LogCategory RYU_CONCAT(CategoryName, Log){ #CategoryName }

#define RYU_LOG_USE_CATEGORY(CategoryName) RYU_CONCAT(CategoryName, Log)

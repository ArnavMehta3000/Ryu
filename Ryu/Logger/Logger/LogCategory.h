#pragma once
#include <string_view>
#include "Common/ObjectMacros.h"

namespace Ryu::Logging
{
	struct LogCategory
	{
		std::string_view Name;

		explicit constexpr LogCategory(std::string_view name) : Name(name) {}
	};
}

#define RYU_LOG_CATEGORY(CategoryName) static constexpr ::Ryu::Logging::LogCategory RYU_CONCAT(CategoryName, Log){ #CategoryName }

#define RYU_USE_LOG_CATEGORY(CategoryName) RYU_CONCAT(CategoryName, Log)

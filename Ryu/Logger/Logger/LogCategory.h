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

#define RYU_LOG_DECLARE_CATEGORY(CategoryName) static inline constexpr ::Ryu::Logging::LogCategory RYU_CONCAT(Log, CategoryName){ #CategoryName }


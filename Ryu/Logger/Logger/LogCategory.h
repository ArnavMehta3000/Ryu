#pragma once
#include <string_view>

namespace Ryu::Logging
{
	struct LogCategory
	{
		std::string_view Name;

		explicit constexpr LogCategory(std::string_view name) : Name(name) {}
	};
}

#define LOG_CATEGORY(CategoryName) static constexpr ::Ryu::Logging::LogCategory CategoryName##Log{ #CategoryName }

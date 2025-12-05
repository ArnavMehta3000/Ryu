module;
#include <string_view>
#include <string>
#include <limits>
#include <algorithm>
#include "Common/API.h"

export module Ryu.Common:Enum;
import :Concepts;

namespace Ryu
{
	template<typename Enum, typename = std::enable_if_t<std::is_enum_v<Enum>>>
	inline constexpr bool EnumHasAllFlags(Enum flags, Enum contains)
	{
		return (((std::underlying_type_t<Enum>)flags) & (std::underlying_type_t<Enum>)contains) == ((std::underlying_type_t<Enum>)contains);
	}

	template<typename Enum, typename = std::enable_if_t<std::is_enum_v<Enum>>>
	inline constexpr bool EnumHasAnyFlags(Enum flags, Enum contains)
	{
		return (((std::underlying_type_t<Enum>)flags) & (std::underlying_type_t<Enum>)contains) != 0;
	}

	template <typename T> requires Ryu::IsEnum<T>
	inline constexpr RYU_API std::string_view EnumToString(T value);

	template <IsEnum EnumType>
	EnumType StringToEnum(const std::string& str, EnumType defaultValue)
	{
		// Try exact match first
		for (int i = 0; i < static_cast<int>(std::numeric_limits<std::underlying_type_t<EnumType>>::max()); ++i)
		{
			if (i > 1000) // Safety limit to prevent infinite loops for really large enums
			{
				break;
			}

			EnumType value = static_cast<EnumType>(i);
			if (std::string(EnumToString(value)) == str)
			{
				return value;
			}
		}

		// Try case-insensitive match
		std::string lowerStr = str;
		std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
			[](unsigned char c) { return std::tolower(c); });

		for (int i = 0; i < static_cast<int>(std::numeric_limits<std::underlying_type_t<EnumType>>::max()); ++i)
		{
			if (i > 1000) // Safety limit
			{
				break;
			}

			EnumType value = static_cast<EnumType>(i);
			std::string enumStr = std::string(EnumToString(value));
			std::transform(enumStr.begin(), enumStr.end(), enumStr.begin(),
				[](unsigned char c) { return std::tolower(c); });

			if (enumStr == lowerStr)
				return value;
		}

		// No match found
		return defaultValue;
	}
}
#pragma once
#include "Common/API.h"
#include "Common/Concepts.h"
#include <string_view>
#include <string>
#include <limits>
#include <algorithm>

template <typename T>
struct EnableBitMaskOps { static constexpr bool Enable = false; };

template <typename T>
concept BitMaskEnabled = std::is_enum_v<T> && EnableBitMaskOps<T>::Enable;

template <BitMaskEnabled Enum>
constexpr std::underlying_type_t<Enum> operator|(Enum lhs, Enum rhs) noexcept
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	return static_cast<UnderlyingType>(lhs) | static_cast<UnderlyingType>(rhs);
}

template <BitMaskEnabled Enum>
constexpr std::underlying_type_t<Enum> operator&(Enum lhs, Enum rhs) noexcept
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	return static_cast<UnderlyingType>(lhs) & static_cast<UnderlyingType>(rhs);
}

template <BitMaskEnabled Enum>
constexpr std::underlying_type_t<Enum> operator^(Enum lhs, Enum rhs) noexcept
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	return static_cast<UnderlyingType>(lhs) ^ static_cast<UnderlyingType>(rhs);
}

template <BitMaskEnabled Enum>
constexpr std::underlying_type_t<Enum> operator~(Enum rhs) noexcept
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	return ~static_cast<UnderlyingType>(rhs);
}

// For assignment operators, we still need to modify the enum value itself
template <BitMaskEnabled Enum>
constexpr Enum& operator|=(Enum& lhs, const Enum rhs) noexcept
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	lhs = static_cast<Enum>(static_cast<UnderlyingType>(lhs) | static_cast<UnderlyingType>(rhs));
	return lhs;
}

template <BitMaskEnabled Enum>
constexpr Enum& operator&=(Enum& lhs, const Enum rhs) noexcept
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	lhs = static_cast<Enum>(static_cast<UnderlyingType>(lhs) & static_cast<UnderlyingType>(rhs));
	return lhs;
}

template <BitMaskEnabled Enum>
constexpr Enum& operator^=(Enum& lhs, const Enum rhs) noexcept
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	lhs = static_cast<Enum>(static_cast<UnderlyingType>(lhs) ^ static_cast<UnderlyingType>(rhs));
	return lhs;
}

#define RYU_ENABLE_BITMASK_OPERATORS(Enum)                                                                                                                               
template<> struct EnableBitMaskOps<Enum> { static constexpr bool Enable = true; };         \
namespace Ryu                                                                              \
{                                                                                          \
	constexpr bool IsEnumMaskBitSet(const Enum mask, const Enum component) noexcept        \
	{                                                                                      \
		return (mask & component) == static_cast<std::underlying_type_t<Enum>>(component); \
	}                                                                                      \
}

namespace Ryu
{
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

#define RYU_ENUM_TO_STRING(EnumType, Body)\
namespace Ryu\
{\
	template<> inline constexpr std::string_view EnumToString<EnumType>(EnumType value) Body\
}

// Used to create a bit mask
#define RYU_BIT(Value) 1 << Value

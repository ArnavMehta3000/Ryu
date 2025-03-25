#pragma once
#include "Common/API.h"
#include "Common/Concepts.h"
#include <string_view>

template <typename T>
struct EnableBitMaskOps { static constexpr bool Enable = false; };

template <typename T>
concept BitMaskEnabled = std::is_enum_v<T> && EnableBitMaskOps<T>::Enable;

template <BitMaskEnabled Enum>
constexpr Enum operator|(Enum lhs, Enum rhs) noexcept
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	return static_cast<Enum>(static_cast<UnderlyingType>(lhs) | static_cast<UnderlyingType>(rhs));
}

template <BitMaskEnabled Enum>
constexpr Enum operator&(Enum lhs, Enum rhs) noexcept
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	return static_cast<Enum>(static_cast<UnderlyingType>(lhs) &static_cast<UnderlyingType>(rhs));
}

template <BitMaskEnabled Enum>
constexpr Enum operator^(Enum lhs, Enum rhs) noexcept
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	return static_cast<Enum>(static_cast<UnderlyingType>(lhs) ^static_cast<UnderlyingType>(rhs));
}

template <BitMaskEnabled Enum>
constexpr Enum operator~(Enum rhs) noexcept
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	return static_cast<Enum>(~static_cast<UnderlyingType>(rhs));
}

template <BitMaskEnabled Enum>
constexpr Enum& operator|=(Enum& lhs, const Enum rhs) noexcept
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	lhs = static_cast<Enum>(static_cast<UnderlyingType>(lhs) |static_cast<UnderlyingType>(rhs));
	return lhs;
}

template <BitMaskEnabled Enum>
constexpr Enum& operator&=(Enum& lhs, const Enum rhs) noexcept
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	lhs = static_cast<Enum>(static_cast<UnderlyingType>(lhs) &static_cast<UnderlyingType>(rhs));
	return lhs;
}

template <BitMaskEnabled Enum>
constexpr Enum& operator^=(Enum& lhs, const Enum rhs) noexcept 
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	lhs = static_cast<Enum>(static_cast<UnderlyingType>(lhs) ^static_cast<UnderlyingType>(rhs));
	return lhs;
}

#define RYU_ENABLE_BITMASK_OPERATORS(Enum)                                                                                     \
template<> struct EnableBitMaskOps<Enum> { static constexpr bool Enable = true; };                                             \
namespace Ryu                                                                                                                  \
{                                                                                                                              \
	constexpr bool IsEnumMaskBitSet(const Enum mask, const Enum component) noexcept { return (mask & component) == component; }\
}

namespace Ryu
{
	template <typename T> requires Ryu::IsEnum<T>
	inline constexpr RYU_API std::string_view EnumToString(T value);
}

// Used to create a bit mask
#define RYU_BIT(Value) 1 << Value

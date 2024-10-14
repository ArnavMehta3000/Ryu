#pragma once
#include <type_traits>

template <typename Enum>
struct EnableBitMaskOps
{
	static constexpr bool Enable = false;
};

template<typename Enum>
constexpr std::enable_if_t<EnableBitMaskOps<Enum>::Enable, Enum> operator|(Enum Lhs, Enum Rhs)
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	return static_cast<Enum>(static_cast<UnderlyingType>(Lhs) | static_cast<UnderlyingType>(Rhs));
}

template<typename Enum>
constexpr std::enable_if_t<EnableBitMaskOps<Enum>::Enable, Enum> operator&(Enum Lhs, Enum Rhs)
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	return static_cast<Enum>(static_cast<UnderlyingType>(Lhs) & static_cast<UnderlyingType>(Rhs));
}

template<typename Enum>
constexpr std::enable_if_t<EnableBitMaskOps<Enum>::Enable, Enum> operator^(Enum Lhs, Enum Rhs)
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	return static_cast<Enum>(static_cast<UnderlyingType>(Lhs) ^ static_cast<UnderlyingType>(Rhs));
}

template<typename Enum>
constexpr std::enable_if_t<EnableBitMaskOps<Enum>::Enable, Enum> operator~(Enum Rhs)
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	return static_cast<Enum>(~static_cast<UnderlyingType>(Rhs));
}

template<typename Enum>
constexpr std::enable_if_t<EnableBitMaskOps<Enum>::Enable, Enum> operator|=(Enum& Lhs, Enum Rhs)
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	Lhs = static_cast<Enum>(static_cast<UnderlyingType>(Lhs) | static_cast<UnderlyingType>(Rhs));
	return Lhs;
}

template<typename Enum>
constexpr std::enable_if_t<EnableBitMaskOps<Enum>::Enable, Enum> operator&=(Enum& Lhs, Enum Rhs)
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	Lhs = static_cast<Enum>(static_cast<UnderlyingType>(Lhs) & static_cast<UnderlyingType>(Rhs));
	return Lhs;
}

template<typename Enum>
constexpr std::enable_if_t<EnableBitMaskOps<Enum>::Enable, Enum> operator^=(Enum& Lhs, Enum Rhs)
{
	using UnderlyingType = std::underlying_type_t<Enum>;
	Lhs = static_cast<Enum>(static_cast<UnderlyingType>(Lhs) ^ static_cast<UnderlyingType>(Rhs));
	return Lhs;
}

#define RYU_ENABLE_BITMASK_OPERATORS(Enum)   \
template<>                                   \
struct EnableBitMaskOps<Enum>                \
{                                            \
	static constexpr bool Enable = true;     \
};                                           \
											 \
namespace Ryu { constexpr bool IsEnumMaskBitSet(Enum Mask, Enum Component) { return (Mask & Component) == Component; } }

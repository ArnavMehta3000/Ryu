#pragma once
#include "Common/API.h"
#include "Common/Concepts.h"
#include <string_view>

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

namespace Ryu
{
	template <typename T> requires Ryu::IsEnum<T>
	inline constexpr RYU_API std::string_view EnumToString(T value);
}

#define RYU_ENUM_TO_STRING(Value) case Value: return RYU_ENUM_STRINGIFY(Value);

#define RYU_ENUM_STRINGIFY(Value) #Value

#define RYU_BEGIN_ENUM_TO_STRING(EnumType)                                     \
	template <>                                                                \
	inline constexpr RYU_API std::string_view Ryu::EnumToString(EnumType value)\
	{                                                                          \
		switch(value)                                                          \
		{                                                                      \
			using enum EnumType;

#define RYU_END_ENUM_TO_STRING()    \
		default: return "<Unknown>";\
		}                           \
	}                               \

#pragma once

#define RYU_ENUM_TO_STRING(EnumType, Body)\
namespace Ryu\
{\
	template<> inline constexpr std::string_view EnumToString<EnumType>(EnumType value) Body\
}

// Used to create a bit mask
#define RYU_ENUM_BIT(Value) 1 << Value

#define RYU_ENUM_ENABLE_BITMASK_OPERATORS(Enum)                                                                                                            \
	inline constexpr Enum& operator|=(Enum& lhs, Enum rhs) { return lhs = (Enum)((std::underlying_type_t<Enum>)lhs | (std::underlying_type_t<Enum>)rhs); } \
	inline constexpr Enum& operator&=(Enum& lhs, Enum rhs) { return lhs = (Enum)((std::underlying_type_t<Enum>)lhs & (std::underlying_type_t<Enum>)rhs); } \
	inline constexpr Enum& operator^=(Enum& lhs, Enum rhs) { return lhs = (Enum)((std::underlying_type_t<Enum>)lhs ^ (std::underlying_type_t<Enum>)rhs); } \
	inline constexpr Enum  operator| (Enum  lhs, Enum rhs) { return (Enum)((std::underlying_type_t<Enum>)lhs | (std::underlying_type_t<Enum>)rhs); }       \
	inline constexpr Enum  operator& (Enum  lhs, Enum rhs) { return (Enum)((std::underlying_type_t<Enum>)lhs & (std::underlying_type_t<Enum>)rhs); }       \
	inline constexpr Enum  operator^ (Enum  lhs, Enum rhs) { return (Enum)((std::underlying_type_t<Enum>)lhs ^ (std::underlying_type_t<Enum>)rhs); }       \
	inline constexpr bool  operator! (Enum  e) { return !(std::underlying_type_t<Enum>)e; }                                                                \
	inline constexpr Enum  operator~ (Enum  e) { return (Enum)~(std::underlying_type_t<Enum>)e; }

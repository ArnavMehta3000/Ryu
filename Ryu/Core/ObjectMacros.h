#pragma once
#include <type_traits>

#define RYU_DISABLE_COPY(T)      \
	explicit T(const T&) = delete; \
	T& operator=(const T&) = delete;

#define RYU_DISABLE_MOVE(T)       \
	explicit T(const T&&) = delete; \
	T& operator=(const T&&) = delete;

#define RYU_DISABLE_COPY_AND_MOVE(T) RYU_DISABLE_COPY(T) RYU_DISABLE_MOVE(T)

// Declares an API function and assigns it to a function pointer
#define RYU_API_FUNC(ret, name, ...) \
	extern "C" RYU_API ret name(__VA_ARGS__);\
	using name##_f = decltype(&name)

#define DEPRECATED [[deprecated]]
#define NODISCARD [[nodiscard]]
#define FALLTHROUGH [[fallthrough]]
#define LIKELY [[likely]]
#define UNLIKELY [[unlikely]]
#define MAYBE_UNUSED [[maybe_unused]]
#define NO_RETURN [[noreturn]]

// Macro to define bitwise operators for enum classes
#define RYU_ENABLE_ENUM_OPERATORS(T)                                                                     \
	constexpr T operator|(T lhs, T rhs)                                                                  \
	{                                                                                                    \
		return static_cast<T>(                                                                           \
			static_cast<std::underlying_type_t<T>>(lhs) | static_cast<std::underlying_type_t<T>>(rhs));  \
	}                                                                                                    \
	constexpr T operator&(T lhs, T rhs)                                                                  \
	{                                                                                                    \
		return static_cast<T>(                                                                           \
			static_cast<std::underlying_type_t<T>>(lhs) & static_cast<std::underlying_type_t<T>>(rhs));  \
	}                                                                                                    \
	constexpr T operator^(T lhs, T rhs)                                                                  \
	{                                                                                                    \
		return static_cast<T>(                                                                           \
			static_cast<std::underlying_type_t<T>>(lhs) ^ static_cast<std::underlying_type_t<T>>(rhs));  \
	}                                                                                                    \
	constexpr T operator~(T lhs)                                                                         \
	{                                                                                                    \
		return static_cast<T>(~static_cast<std::underlying_type_t<T>>(lhs));                             \
	}                                                                                                    \
	inline T& operator|=(T& lhs, T rhs)                                                                  \
	{                                                                                                    \
		lhs = lhs | rhs;                                                                                 \
		return lhs;                                                                                      \
	}                                                                                                    \
	inline T& operator&=(T& lhs, T rhs)                                                                  \
	{                                                                                                    \
		lhs = lhs & rhs;                                                                                 \
		return lhs;                                                                                      \
	}                                                                                                    \
	inline T& operator^=(T& lhs, T rhs)                                                                  \
	{                                                                                                    \
		lhs = lhs ^ rhs;                                                                                 \
		return lhs;                                                                                      \
	}


// Macro to define bitwise operators for enum classes, returning the underlying type
#define RYU_ENABLE_ENUM_OPERATORS_UNDERLYING(T)                                                           \
	constexpr std::underlying_type_t<T> operator|(T lhs, T rhs)                                           \
	{                                                                                                     \
		return static_cast<std::underlying_type_t<T>>(lhs) | static_cast<std::underlying_type_t<T>>(rhs); \
	}                                                                                                     \
	constexpr std::underlying_type_t<T> operator&(T lhs, T rhs)                                           \
	{                                                                                                     \
		return static_cast<std::underlying_type_t<T>>(lhs) & static_cast<std::underlying_type_t<T>>(rhs); \
	}                                                                                                     \
	constexpr std::underlying_type_t<T> operator^(T lhs, T rhs)                                           \
	{                                                                                                     \
		return static_cast<std::underlying_type_t<T>>(lhs) ^ static_cast<std::underlying_type_t<T>>(rhs); \
	}                                                                                                     \
	constexpr std::underlying_type_t<T> operator~(T lhs)                                                  \
	{                                                                                                     \
		return ~static_cast<std::underlying_type_t<T>>(lhs);                                              \
	}                                                                                                     \
	inline std::underlying_type_t<T>& operator|=(T& lhs, T rhs)                                           \
	{                                                                                                     \
		auto& ulhs = reinterpret_cast<std::underlying_type_t<T>&>(lhs);                                   \
		ulhs |= static_cast<std::underlying_type_t<T>>(rhs);                                              \
		return ulhs;                                                                                      \
	}                                                                                                     \
	inline std::underlying_type_t<T>& operator&=(T& lhs, T rhs)                                           \
	{                                                                                                     \
		auto& ulhs = reinterpret_cast<std::underlying_type_t<T>&>(lhs);                                   \
		ulhs &= static_cast<std::underlying_type_t<T>>(rhs);                                              \
		return ulhs;                                                                                      \
	}                                                                                                     \
	inline std::underlying_type_t<T>& operator^=(T& lhs, T rhs)                                           \
	{                                                                                                     \
		auto& ulhs = reinterpret_cast<std::underlying_type_t<T>&>(lhs);                                   \
		ulhs ^= static_cast<std::underlying_type_t<T>>(rhs);                                              \
		return ulhs;                                                                                      \
	}
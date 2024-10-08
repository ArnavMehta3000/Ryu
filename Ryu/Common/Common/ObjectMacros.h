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

#define RYU_EMIT_COMPILER_WARNING_STRINGIFY0(x) #x
#define RYU_EMIT_COMPILER_WARNING_STRINGIFY1(x) RYU_EMIT_COMPILER_WARNING_STRINGIFY0(x)
#define RYU_EMIT_COMPILER_MESSAGE_PREFACE(type) __FILE__ "(" RYU_EMIT_COMPILER_WARNING_STRINGIFY1(__LINE__) "): " type ": "
#define RYU_EMIT_COMPILER_WARNING_COMPOSE(x) message(RYU_EMIT_COMPILER_MESSAGE_PREFACE("warning RYU0000") x)
#define RYU_EMIT_WARNING(x) _Pragma(RYU_EMIT_COMPILER_WARNING_STRINGIFY1(RYU_EMIT_COMPILER_WARNING_COMPOSE(x)))

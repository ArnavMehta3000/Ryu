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

#define RYU_CONCAT_IMPL(x, y) x##y
#define RYU_CONCAT(x, y) RYU_CONCAT_IMPL(x, y)

#define RYU_STRING_IMPL(x) #x
#define RYU_STRING(x) RYU_STRING_IMPL(x)

// Macro to print a TODO message
#define RYU_TODO(msg) __pragma(message(__FILE__ "(" RYU_STRING(__LINE__) "): TODO: " msg))

// Macro to print a warning
#define RYU_NOTE(msg) __pragma(message(__FILE__ "(" RYU_STRING(__LINE__) "): NOTE: " msg))

// Macro to say that the function is not implemented
#define RYU_NOT_IMPLEMENTED(LogCategory) RYU_LOG_WARN(LogCategory, "Function {} not implemented", std::string_view(__FUNCTION__))

// Checks if a function exists for a given class
#define RYU_FUNCTION_CHECK(NAME, ...) namespace Ryu { template<typename T> class HasFunction_##NAME {                        \
	template<typename U> constexpr static auto Check(int) -> decltype(std::declval<U>().__VA_ARGS__, bool()) { return true; }\
	template<typename> constexpr static bool Check(...) { return false; } public:                                            \
	static constexpr bool Value = Check<T>(0); }; }

// Only executes code if RYU_BUILD_DEBUG is defined
#if defined(RYU_BUILD_DEBUG)
#define RYU_DEBUG_OP(X) X
#else
#define RYU_DEBUG_OP(X) do {} while (false)
#endif

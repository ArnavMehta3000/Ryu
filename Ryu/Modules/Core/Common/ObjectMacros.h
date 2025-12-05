#pragma once
#include <type_traits>
#include <string_view>

#define RYU_DISABLE_COPY(T)        \
	explicit T(const T&) = delete; \
	T& operator=(const T&) = delete;

#define RYU_DISABLE_MOVE(T)         \
	explicit T(const T&&) = delete; \
	T& operator=(const T&&) = delete;

#define RYU_DISABLE_COPY_AND_MOVE(T) RYU_DISABLE_COPY(T) RYU_DISABLE_MOVE(T)

// Declares an API function and assigns it to a function pointer
#define RYU_API_FUNC(ret, name, ...)         \
	extern "C" RYU_API ret name(__VA_ARGS__);\
	using name##_f = decltype(&name)

#define RYU_DEBUGBREAK() __debugbreak()

#define RYU_STRING_IMPL(x) #x
#define RYU_STRING(x) RYU_STRING_IMPL(x)

#define RYU_PRAGMA(PRAGMA)          _Pragma(#PRAGMA)
#define RYU_PRAGMA_MESSAGE(MESSAGE) RYU_PRAGMA(message(MESSAGE))

#define RYU_EMIT_MESSAGE(MESSAGE)   RYU_PRAGMA_MESSAGE(__FILE__ "(" RYU_STRING(__LINE__) "): " MESSAGE)
#define RYU_EMIT_WARNING(MESSAGE) RYU_EMIT_MESSAGE("warning: " MESSAGE)
#define RYU_EMIT_ERROR(MESSAGE)   static_assert(false, "error: " MESSAGE)

#define RYU_CONCAT_IMPL(x, y) x##y
#define RYU_CONCAT(x, y) RYU_CONCAT_IMPL(x, y)

// Macro to print a TODO message
#define RYU_TODO(msg) __pragma(message(__FILE__ "(" RYU_STRING(__LINE__) "): TODO: " msg))

// Macro to print a warning
#define RYU_NOTE(msg) __pragma(message(__FILE__ "(" RYU_STRING(__LINE__) "): NOTE: " msg))

// Macro to say that the function is not implemented
#define RYU_NOT_IMPLEMENTED() RYU_LOG_WARN("Function {} not implemented", std::string_view(__FUNCTION__)); RYU_DEBUGBREAK()

// Only executes code if RYU_BUILD_DEBUG is defined
#if defined(RYU_BUILD_DEBUG)
#define RYU_DEBUG_OP(X) X
#else
#define RYU_DEBUG_OP(X) do {} while (false)
#endif

// Only executes code if RYU_BUILD_DEBUG is defined
#if defined(RYU_BUILD_DEBUG)
#define RYU_DEBUG_BLOCK(...) __VA_ARGS__
#else
#define RYU_DEBUG_BLOCK(...)
#endif

#define RYU_PROPERTY(Name, ...) __declspec(property(get = RYU_CONCAT(Get, Name), put = RYU_CONCAT(Set, Name))) __VA_ARGS__ Name
#define RYU_GET_ONLY_PROPERTY(Name, ...) __declspec(property(get = RYU_CONCAT(Get, Name))) __VA_ARGS__ Name
#define RYU_SET_ONLY_PROPERTY(Name, ...) __declspec(property(put = RYU_CONCAT(Set, Name))) __VA_ARGS__ Name

// Helper macro to generate getter functions with attributes
#define RYU_GETTER_FUNC(ReturnType, FuncName, ReturnValue) [[nodiscard]] inline ReturnType FuncName() const noexcept { return ReturnValue; }

// Empty macro to make code blocks easier to read
#define RYU_CODE_BLOCK(Name)

// --- Functions below are in the module ---

// The filename with extension of the current file
#define RYU_FILENAME ::Ryu::ExtractFilename(__FILE__)

// The filename without extension of the current file
#define RYU_FILE ::Ryu::ExtractFilenameNoExt(__FILE__)

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
#define RYU_NOT_IMPLEMENTED() RYU_LOG_WARN("Function {} not implemented", std::string_view(__FUNCTION__))

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

// Empty macro to make code blocks easier to read
#define RYU_CODE_BLOCK(Name)

namespace Ryu
{
	constexpr std::string_view ExtractFilename(std::string_view path) noexcept
	{
		// Find the last occurrence of either '/' or '\'
		size_t lastSlash = path.find_last_of("/\\");

		// No slash found, return the entire string (it's already just a filename)
		if (lastSlash == std::string_view::npos)
		{
			return path;
		}

		// Return substring starting after the last slash
		return path.substr(lastSlash + 1);
	}

	constexpr std::string_view ExtractFilenameNoExt(std::string_view path) noexcept
	{
		std::string_view filename = ExtractFilename(path);

		// Find last dot
		size_t lastDot = filename.find_last_of('.');

		// No dot found, return the entire filename
		if (lastDot == std::string_view::npos)
		{
			return filename;
		}

		// Return substring up to (but not including) the last dot
		return filename.substr(0, lastDot);
	}
}

// The filename with extension of the current file
#define RYU_FILENAME ::Ryu::ExtractFilename(__FILE__)

// The filename without extension of the current file
#define RYU_FILE ::Ryu::ExtractFilenameNoExt(__FILE__)

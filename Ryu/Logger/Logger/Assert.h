#pragma once
#include "Logger/LogCategory.h"
#include <stdexcept>
#include <source_location>
#include <stacktrace>
#include <format>

#if defined(RYU_BUILD_DEBUG)
#define RYU_ASSERT_ENABLED 1
#else
#define RYU_ASSERT_ENABLED 0
#endif


namespace Ryu
{
    RYU_LOG_DECLARE_CATEGORY(Assert);

    class Exception : public std::exception
    {
    public:
        // Constructor with condition expression and optional message
        Exception(std::string_view condition,
            std::string_view message = "",
            const std::source_location& location = std::source_location::current(),
            std::stacktrace trace = std::stacktrace::current())
            : m_trace(std::move(trace))
        {
            if (message.empty())
            {
                m_message = std::format("Assertion '{}' failed at {}:{}:{}",
                    condition,
                    location.file_name(),
                    location.line(),
                    location.column());
            }
            else
            {
                m_message = std::format("Assertion '{}' failed: {} at {}:{}:{}",
                    condition,
                    message,
                    location.file_name(),
                    location.line(),
                    location.column());
            }
        }

        const char* what() const noexcept override
        {
            return m_message.c_str();
        }

        const std::stacktrace& GetTrace() const noexcept
        {
            return m_trace;
        }

    private:
        std::string m_message;
        std::stacktrace m_trace;
    };


#if RYU_ASSERT_ENABLED
    namespace Internal
    {
        inline void AssertImpl(bool result,
            std::string_view condition,
            std::string_view message = "",
            const std::source_location& location = std::source_location::current())
        {
            if (!result)
            {
                throw ::Ryu::Exception(condition, message, location);
            }
        }
    }
#endif
}


#if RYU_ASSERT_ENABLED
#define RYU_INTERNAL_GET_MACRO(_1, _2, NAME, ...) NAME

#define RYU_ASSERT(...) RYU_INTERNAL_EXPAND(RYU_INTERNAL_GET_MACRO(__VA_ARGS__, RYU_INTERNAL_ASSERT_WITH_MSG, RYU_INTERNAL_ASSERT_NO_MSG)(__VA_ARGS__))

#define RYU_INTERNAL_EXPAND(x) x

#define RYU_INTERNAL_ASSERT_NO_MSG(condition) \
    ::Ryu::Internal::AssertImpl(!!(condition), #condition, "", std::source_location::current())

#define RYU_INTERNAL_ASSERT_WITH_MSG(condition, message) \
    ::Ryu::Internal::AssertImpl(!!(condition), #condition, message, std::source_location::current())

#else
#define RYU_ASSERT(...) do {} while (0)
#endif

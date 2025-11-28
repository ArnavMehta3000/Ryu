#pragma once
#include <stdexcept>
#include <source_location>
#include <stacktrace>
#include <format>
#include <functional>

#if defined(RYU_BUILD_DEBUG)
#define RYU_ASSERT_ENABLED 1
#else
#define RYU_ASSERT_ENABLED 0
#endif

namespace Ryu
{
    namespace Logging { struct LogCategory; }

    class AssertException : public std::exception
    {
    public:
        AssertException(std::string_view condition,
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

        const std::string& Message() const noexcept
        {
            return m_message;
        }

    private:
        std::string m_message;
        std::stacktrace m_trace;
    };

    using AssertHandler = std::function<void(const AssertException&)>;

    class AssertManager
    {
    public:
        static void SetAssertHandler(AssertHandler handler)
        {
            s_handler = handler;
        }

        static void HandleAssert(const AssertException& exception)
        {
            if (s_handler)
            {
                std::invoke(s_handler, exception);
            }
            else
            {
                // Default behavior: throw the exception
                throw exception;
            }
        }

    private:
        static inline AssertHandler s_handler;
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
                AssertException exception(condition, message, location);
                AssertManager::HandleAssert(exception);
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

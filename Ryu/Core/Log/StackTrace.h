#pragma once
#include <memory>
#include <string>

namespace backward
{
    class StackTrace;
}

namespace Ryu
{
    class StackTrace
    {
    public:
        StackTrace();
        StackTrace(const StackTrace& other);
        StackTrace& operator=(const StackTrace& other);
        ~StackTrace();

        std::string Print() const;

    private:
        std::unique_ptr<backward::StackTrace> m_trace;
    };
}

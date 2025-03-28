#pragma once
#include "Common/ObjectMacros.h"

namespace Ryu::Utils
{
    template<typename Callback>
    class Deferred
    {
    public:
        Deferred(Callback&& callback)
            : m_callback(callback)
        {
        }

        ~Deferred() { m_callback(); }

    private:
        Callback m_callback;
    };
}

module;
export module Ryu.Core.Utils:Deferred;

namespace Ryu::Utils
{
    export template<typename Callback>
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

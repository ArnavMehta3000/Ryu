#pragma once
#include <typeindex>

namespace Ryu::Event
{
	class EventDispatcher;

	class ListenerHandle
	{
		friend class EventDispatcher;
	public:
		ListenerHandle()
			: m_eventType(typeid(void))
			, m_listenerId(0)
			, m_isValid(false)
		{}

		ListenerHandle(std::type_index type, u64 id)
			: m_eventType(type)
			, m_listenerId(id)
			, m_isValid(true)
		{}

		operator bool() { return m_isValid; }
		inline bool IsValid() const { return m_isValid; }

	private:
		std::type_index m_eventType;
		u64 m_listenerId;
		bool m_isValid;
	};
}

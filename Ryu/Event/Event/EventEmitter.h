#pragma once
#include "Common/ObjectMacros.h"
#include "Event/Event.h"
#include "Event/EventDispatcher.h"
#include "Event/ListenerHandle.h"
#include <memory>

namespace Ryu::Event
{
	class EventEmitter
	{
	public:
		template<typename EventType, typename CallbackType>
		ListenerHandle On(CallbackType&& callback)
		{
			return m_dispatcher.Subscribe<EventType>(std::forward<CallbackType>(callback));
		}

		void Unsubscribe(const ListenerHandle& handle)
		{
			m_dispatcher.Unsubscribe(handle);
		}

		template<typename EventType>
		void Emit(const EventType& event)
		{
			m_dispatcher.Dispatch(event);
		}

		template<typename EventType>
		void QueueEmit(std::unique_ptr<EventType> event)
		{
			m_dispatcher.QueueEvent(std::move(event));
		}

		void ProcessEventQueue()
		{
			m_dispatcher.ProcessQueue();
		}

		NODISCARD EventDispatcher& GetDispatcher() { return m_dispatcher; }
		NODISCARD const EventDispatcher& GetDispatcher() const { return m_dispatcher; }

	protected:
		EventDispatcher m_dispatcher;
	};
}

#pragma once
#include "Event/Event.h"
#include "ListenerHandle.h"
#include "Common/StandardTypes.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>

namespace Ryu::Event
{
	class EventDispatcher
	{
		using ListenerFunc = std::function<void(const Event&)>;

		struct ListenerEntry
		{
			ListenerFunc Callback;
			u64 Id;
			bool IsActive = true;
		};

	public:
		template<IsEvent EventType, typename CallbackType>
		ListenerHandle Subscribe(CallbackType&& callback);

		void Unsubscribe(const ListenerHandle& handle);

		template<IsEvent EventType>
		void Dispatch(const EventType& event);

		template<IsEvent EventType>
		void QueueEvent(std::unique_ptr<EventType> event);

		void ProcessQueue();

		template <IsEvent EventType>
		u64 GetListenerCount() const;

	private:
		void DispatchPolymorphic(const Event& event);
		void CleanupInactiveListeners();

	private:
		std::unordered_map<std::type_index, std::vector<ListenerEntry>> m_listeners;
		u64 m_nextListenerId = 0;
		bool m_dispatching = false;
		std::vector<std::type_index> m_cleanupQueue;
		std::vector<std::unique_ptr<Event>> m_eventQueue;
	};
}

#include "Event/EventDispatcher.inl"

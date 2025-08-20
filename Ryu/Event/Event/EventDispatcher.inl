#include <algorithm>

namespace Ryu::Event
{
	template<IsEvent EventType, typename CallbackType>
	inline ListenerHandle EventDispatcher::Subscribe(CallbackType&& callback)
	{
		auto typeIndex = std::type_index(typeid(EventType));
		auto listenerId = m_nextListenerId++;

		// Wrap callback to cast event to correct type
		auto wrappedCallback = [callback = std::forward<CallbackType>(callback)](const Event& event)
		{
			callback(static_cast<const EventType&>(event));
		};

		m_listeners[typeIndex].emplace_back(ListenerEntry
		{
			.Callback = std::move(wrappedCallback),
			.Id = listenerId,
			.IsActive = true
		});

		return ListenerHandle(typeIndex, listenerId);
	}

	template<IsEvent EventType>
	inline void EventDispatcher::Dispatch(const EventType& event)
	{
		auto typeIndex = std::type_index(typeid(EventType));
		auto it = m_listeners.find(typeIndex);

		if (it == m_listeners.end())
		{
			return;
		}

		m_dispatching = true;

		const auto& listenersList = it->second;
		for (auto& listener : listenersList)
		{
			listener.Callback(event);
			if (event.Handled)
			{
				break;  // Allow events to be consumed
			}
		}

		m_dispatching = false;
		CleanupInactiveListeners();
	}

	template<IsEvent EventType>
	inline void EventDispatcher::QueueEvent(std::unique_ptr<EventType> event)
	{
		m_eventQueue.emplace_back(std::move(event));
	}

	template <IsEvent EventType>
	inline u64 EventDispatcher::GetListenerCount() const
	{
		auto typeIndex = std::type_index(typeid(EventType));
		auto it = m_listeners.find(typeIndex);

		// Return the number of active listeners
		return (it != m_listeners.end()) ?
			std::count_if(it->second.begin(), it->second.end(),
				[](const ListenerEntry& entry) { return entry.IsActive; }) : 0;
	}
}

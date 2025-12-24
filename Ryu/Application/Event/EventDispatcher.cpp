#include "Application/Event/EventDispatcher.h"

namespace Ryu::Event
{
	void EventDispatcher::Unsubscribe(const ListenerHandle& handle)
	{
		auto it = m_listeners.find(handle.m_eventType);

		if (it != m_listeners.end())
		{
			auto& listenersList = it->second;
			auto listenerIt = std::find_if(listenersList.begin(), listenersList.end(),
				[&handle](const ListenerEntry& entry) { return entry.Id == handle.m_listenerId; });

			if (listenerIt != listenersList.end())
			{
				if (m_dispatching)  // Mark for removal during cleanup
				{
					listenerIt->IsActive = false;
					m_cleanupQueue.push_back(handle.m_eventType);
				}
				else
				{
					listenersList.erase(listenerIt);
				}
			}
		}
	}

	void EventDispatcher::ProcessQueue()
	{
		if (m_eventQueue.size() > 0)
		{
			for (auto& event : m_eventQueue)
			{
				DispatchPolymorphic(*event);
			}
			m_eventQueue.clear();
		}
	}

	void EventDispatcher::DispatchPolymorphic(const Event& event)
	{
		auto typeIndex = std::type_index(typeid(event));
		auto it = m_listeners.find(typeIndex);

		if (it == m_listeners.end())
		{
			return;
		}

		m_dispatching = true;

		auto& listenerList = it->second;
		for (auto& listener : listenerList)
		{
			if (listener.IsActive)
			{
				listener.Callback(event);
				if (event.Handled)
				{
					break;  // Allow events to be consumed
				}
			}
		}

		m_dispatching = false;
		CleanupInactiveListeners();
	}

	void EventDispatcher::CleanupInactiveListeners()
	{
		for (auto& typeIndex : m_cleanupQueue)
		{
			auto it = m_listeners.find(typeIndex);
			if (it != m_listeners.end())
			{
				auto& listenersList = it->second;

				// Remove inactive listeners
				listenersList.erase(
					std::remove_if(listenersList.begin(), listenersList.end(),
						[](const ListenerEntry& entry) { return !entry.IsActive; }),
					listenersList.end());
			}
		}

		m_cleanupQueue.clear();
	}
}

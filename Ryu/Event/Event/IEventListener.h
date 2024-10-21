#pragma once

namespace Ryu::Events
{
	namespace Internal
	{
		// Base class for all event listeners
		class EventListenerBase
		{
		public:
			virtual ~EventListenerBase() = default;
		};
	}

	// Base class for all event listeners
	template <typename EventType>
	class IEventListener : public Internal::EventListenerBase
	{
	public:
		virtual ~IEventListener() = default;
		virtual void OnEvent(const EventType& event) = 0;
	};
}

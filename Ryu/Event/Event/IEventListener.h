#pragma once

namespace Ryu::Events
{
	namespace Internal
	{
		class EventListenerBase
		{
		public:
			virtual ~EventListenerBase() = default;
		};
	}

	
	/**
	 * @brief Base class for all event listeners
	 * @tparam EventType The type of the event callback to receive
	 * @details Derived classes must implement the `OnEvent` method to receive events
	 */
	template <typename EventType>
	class IEventListener : public Internal::EventListenerBase
	{
	public:
		/**
		 * @brief Virtual destructor
		 */
		virtual ~IEventListener() = default;

		/**
		 * @brief Callback to receive events
		 * @param event The templated event
		 */
		virtual void OnEvent(const EventType& event) = 0;
	};
}

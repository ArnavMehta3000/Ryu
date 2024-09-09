#pragma once
#include <CoreDLLDefines.h>

namespace Ryu
{
	namespace Internal
	{
		// Base class for all event listeners
		class RYU_API EventListenerBase
		{
		public:
			virtual ~EventListenerBase() = default;
		};
	}

	// Base class for all event listeners
	template <typename EventType>
	class RYU_API EventListener : public Internal::EventListenerBase
	{
	public:
		virtual ~EventListener() = default;
		virtual void OnEvent(const EventType& event) = 0;
	};
}
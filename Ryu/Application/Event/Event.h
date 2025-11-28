#pragma once
#include <type_traits>

namespace Ryu::Event
{
	struct Event
	{
		virtual ~Event() = default;
		bool Handled = false;
	};

	template <typename EventType>
	concept IsEvent = std::is_base_of_v<Event, EventType>;
}

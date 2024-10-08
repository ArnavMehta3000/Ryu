#pragma once
#include "Common/Common.h"
#include "Event/Events.h"
#include "Input/KeyCode.h"

namespace Ryu::Input
{
	namespace Events
	{
		namespace Internal
		{
			struct OnKeyEvent
			{
				KeyCode Key;
			};
		}

		struct OnKeyDown : public Internal::OnKeyEvent
		{
			explicit OnKeyDown(KeyCode key)
			{
				Key = key;
			}
		};

		struct OnKeyUp : public Internal::OnKeyEvent
		{
			explicit OnKeyUp(KeyCode key)
			{
				Key = key;
			}
		};
	}

	// Abstract interface that only listens to keyboard events
	class IKeyboardEventListener
		: public Ryu::Events::IEventListener<Events::OnKeyDown>
		, public Ryu::Events::IEventListener<Events::OnKeyUp>
	{
	public:
		virtual void OnEvent(MAYBE_UNUSED const Ryu::Input::Events::OnKeyDown& event) = 0;
		virtual void OnEvent(MAYBE_UNUSED const Ryu::Input::Events::OnKeyUp& event) = 0;
	};
}

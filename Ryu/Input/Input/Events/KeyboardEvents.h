#pragma once
#include "Common/Common.h"
#include "Event/IEventListener.h"
#include "Input/KeyCode.h"

namespace Ryu::Input
{
	namespace Events
	{
		namespace Internal
		{
			struct RYU_API OnKeyEvent
			{
				KeyCode Key;
			};
		}

		struct RYU_API OnKeyDown : public Internal::OnKeyEvent
		{
			explicit OnKeyDown(KeyCode key)
			{
				Key = key;
			}
		};

		struct RYU_API OnKeyUp : public Internal::OnKeyEvent
		{
			explicit OnKeyUp(KeyCode key)
			{
				Key = key;
			}
		};
	}

	// Abstract interface that only listens to keyboard events
	class RYU_API IKeyboardEventListener
		: public Ryu::Events::IEventListener<Events::OnKeyDown>
		, public Ryu::Events::IEventListener<Events::OnKeyUp>
	{
	public:
		virtual void OnEvent(MAYBE_UNUSED const Ryu::Input::Events::OnKeyDown& event) = 0;
		virtual void OnEvent(MAYBE_UNUSED const Ryu::Input::Events::OnKeyUp& event) = 0;
	};
}

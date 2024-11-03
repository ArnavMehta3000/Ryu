#pragma once
#include "Input/Events/KeyboardEvents.h"
#include "Input/Events/MouseEvents.h"

namespace Ryu::Input
{
	// Abstract interface that listens to both keyboard and mouse events
	class RYU_API IInputEventListener
		: public IKeyboardEventListener
		, public IMouseEventListener
	{
	};
}

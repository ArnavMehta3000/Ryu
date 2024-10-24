#pragma once
#include "Common/ObjectMacros.h"
#include "Event/IEventListener.h"

namespace Ryu::App
{
	class Window;

	enum class WindowState
	{
		Restored  = 0,  // When window has been resized, but minimized or maximized doesn't apply
		Maximized = 1,  // When window is maximized
		Minimized = 2   // When window is minimized
	};

	enum class WindowFocus
	{
		Gained,  // This window just gained focus
		Lost     // This window just lost focus
	};

	namespace Events
	{
		namespace Internal
		{
			struct WindowEvent
			{
				Window* Window{ nullptr };
			};
		}

		struct OnWindowClose : public Internal::WindowEvent {};

		struct OnWindowStateChange : public Internal::WindowEvent
		{
			WindowState State{ WindowState::Restored };
			u32 Width{ 0 };
			u32 Height{ 0 };
		};

		struct OnWindowResize : public Internal::WindowEvent
		{
			u32 Width{ 0 };
			u32 Height{ 0 };
		};

		struct OnWindowFocusChange : public Internal::WindowEvent
		{
			WindowFocus Focus{ WindowFocus::Lost };
		};
	}
}

RYU_BEGIN_ENUM_TO_STRING(Ryu::App::WindowState)
	RYU_ENUM_TO_STRING(Restored)
	RYU_ENUM_TO_STRING(Maximized)
	RYU_ENUM_TO_STRING(Minimized)
RYU_END_ENUM_TO_STRING()

RYU_BEGIN_ENUM_TO_STRING(Ryu::App::WindowFocus)
	RYU_ENUM_TO_STRING(Gained)
	RYU_ENUM_TO_STRING(Lost)
RYU_END_ENUM_TO_STRING()

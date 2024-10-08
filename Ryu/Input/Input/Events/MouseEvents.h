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
			struct OnMouseEvent
			{
				MouseButton Button;
				i32 PosX;
				i32 PosY;
			};
		}

		struct OnMouseButtonDown : public Internal::OnMouseEvent
		{
			explicit OnMouseButtonDown(MouseButton button, i32 x, i32 y)
			{
				Button = button;
				PosX = x;
				PosY = y;
			}
		};

		struct OnMouseButtonUp : public Internal::OnMouseEvent
		{
			explicit OnMouseButtonUp(MouseButton button, i32 x, i32 y)
			{
				Button = button;
				PosX = x;
				PosY = y;
			}
		};

		struct OnMouseDblClick : public Internal::OnMouseEvent
		{
			explicit OnMouseDblClick(MouseButton button, i32 x, i32 y)
			{
				Button = button;
				PosX = x;
				PosY = y;
			}
		};

		struct OnMouseMove : public Internal::OnMouseEvent
		{
			explicit OnMouseMove(MouseButton button, i32 x, i32 y)
			{
				Button = button;
				PosX = x;
				PosY = y;
			}
		};

		struct OnMouseMoveRaw : public Internal::OnMouseEvent
		{
			explicit OnMouseMoveRaw(MouseButton button, i32 x, i32 y)
			{
				Button = button;
				PosX = x;
				PosY = y;
			}
		};

		struct OnMouseWheel : public Internal::OnMouseEvent
		{
			i32 Delta;

			explicit OnMouseWheel(MouseButton button, i32 delta, i32 x, i32 y)
			{
				Button = button;
				Delta = delta;
				PosX = x;
				PosY = y;
			}
		};
	}

	// Abstract interface that only listens to mouse events
	class IMouseEventListener
		: public Ryu::Events::IEventListener<Ryu::Input::Events::OnMouseButtonUp>
		, public Ryu::Events::IEventListener<Ryu::Input::Events::OnMouseButtonDown>
		, public Ryu::Events::IEventListener<Ryu::Input::Events::OnMouseDblClick>
		, public Ryu::Events::IEventListener<Ryu::Input::Events::OnMouseMove>
		, public Ryu::Events::IEventListener<Ryu::Input::Events::OnMouseMoveRaw>
		, public Ryu::Events::IEventListener<Ryu::Input::Events::OnMouseWheel>
	{
	public:
		virtual void OnEvent(MAYBE_UNUSED const Ryu::Input::Events::OnMouseButtonUp& event)   = 0;
		virtual void OnEvent(MAYBE_UNUSED const Ryu::Input::Events::OnMouseButtonDown& event) = 0;
		virtual void OnEvent(MAYBE_UNUSED const Ryu::Input::Events::OnMouseDblClick& event)   = 0;
		virtual void OnEvent(MAYBE_UNUSED const Ryu::Input::Events::OnMouseMove& event)       = 0;
		virtual void OnEvent(MAYBE_UNUSED const Ryu::Input::Events::OnMouseMoveRaw& event)    = 0;
		virtual void OnEvent(MAYBE_UNUSED const Ryu::Input::Events::OnMouseWheel& event)      = 0;
	};
}

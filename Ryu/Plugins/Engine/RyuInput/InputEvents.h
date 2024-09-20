#pragma once
#include <Core/ObjectMacros.h>
#include <Plugins/Engine/RyuInput/KeyCode.h>
#include <Core/Event/EventListener.h>
#include <functional>

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
			
			struct RYU_API OnMouseEvent
			{
				MouseButton Button;
				i32 PosX;
				i32 PosY;
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

		struct RYU_API OnMouseButtonDown : public Internal::OnMouseEvent
		{
			explicit OnMouseButtonDown(MouseButton button, i32 x, i32 y)
			{
				Button = button;
				PosX = x;
				PosY = y;
			}
		};

		struct RYU_API OnMouseButtonUp : public Internal::OnMouseEvent
		{
			explicit OnMouseButtonUp(MouseButton button, i32 x, i32 y)
			{
				Button = button;
				PosX = x;
				PosY = y;
			}
		};

		struct RYU_API OnMouseDblClick : public Internal::OnMouseEvent
		{
			explicit OnMouseDblClick(MouseButton button, i32 x, i32 y)
			{
				Button = button;
				PosX = x;
				PosY = y;
			}
		};

		struct RYU_API OnMouseMove: public Internal::OnMouseEvent
		{
			explicit OnMouseMove(MouseButton button, i32 x, i32 y)
			{
				Button = button;
				PosX = x;
				PosY = y;
			}
		};

		struct RYU_API OnMouseMoveRaw : public Internal::OnMouseEvent
		{
			explicit OnMouseMoveRaw(MouseButton button, i32 x, i32 y)
			{
				Button = button;
				PosX = x;
				PosY = y;
			}
		};

		struct RYU_API OnMouseWheel : public Internal::OnMouseEvent
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

	using OnKeyDownCallback         = std::function<void(const Events::OnKeyDown&)>;
	using OnKeyUpCallback           = std::function<void(const Events::OnKeyUp&)>;
	using OnMouseButtonUpCallback   = std::function<void(const Events::OnMouseButtonUp&)>;
	using OnMouseButtonDownCallback = std::function<void(const Events::OnMouseButtonDown&)>;
	using OnMouseDblClickCallback   = std::function<void(const Events::OnMouseDblClick&)>;
	using OnMouseMoveCallback       = std::function<void(const Events::OnMouseMove&)>;
	using OnMouseMoveRawCallback    = std::function<void(const Events::OnMouseMoveRaw&)>;
	using OnMouseWheelCallback      = std::function<void(const Events::OnMouseWheel&)>;

	struct RYU_API InputCallbacks
	{
		OnKeyDownCallback OnKeyDown;
		OnKeyUpCallback OnKeyUp;
		OnMouseButtonUpCallback OnMouseButtonUp;
		OnMouseButtonDownCallback OnMouseButtonDown;
		OnMouseDblClickCallback OnMouseDblClick;
		OnMouseMoveCallback OnMouseMove;
		OnMouseMoveRawCallback OnMouseMoveRaw;
		OnMouseWheelCallback OnMouseWheel;

		InputCallbacks() = default;
		~InputCallbacks() = default;
	};


	// Base class that only listens to keyboard events
	class RYU_API KeyboardEventListener
		: public EventListener<Input::Events::OnKeyDown>
		, public EventListener<Input::Events::OnKeyUp>
	{
	protected:
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnKeyDown& event) {}
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnKeyUp& event) {}
	};

	// Base class that only listens to mouse events
	class RYU_API MouseEventListener
		: public EventListener<Input::Events::OnMouseButtonUp>
		, public EventListener<Input::Events::OnMouseButtonDown>
		, public EventListener<Input::Events::OnMouseDblClick>
		, public EventListener<Input::Events::OnMouseMove>
		, public EventListener<Input::Events::OnMouseMoveRaw>
		, public EventListener<Input::Events::OnMouseWheel>
	{
	protected:
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseButtonUp& event) {}
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseButtonDown& event) {}
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseDblClick& event) {}
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseMove& event) {}
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseMoveRaw& event) {}
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseWheel& event) {}
	};

	// Base class that listens to all input devices events
	class RYU_API InputEventListener 
		: public KeyboardEventListener
		, public MouseEventListener
	{

	};
}

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
	}

	using OnKeyDownCallback         = std::function<void(const Events::OnKeyDown&)>;
	using OnKeyUpCallback           = std::function<void(const Events::OnKeyUp&)>;
	using OnMouseButtonUpCallback   = std::function<void(const Events::OnMouseButtonUp&)>;
	using OnMouseButtonDownCallback = std::function<void(const Events::OnMouseButtonDown&)>;
	using OnMouseDblClickCallback   = std::function<void(const Events::OnMouseDblClick&)>;


	struct RYU_API InputCallbacks
	{
		OnKeyDownCallback OnKeyDown;
		OnKeyUpCallback OnKeyUp;
		OnMouseButtonUpCallback OnMouseButtonUp;
		OnMouseButtonDownCallback OnMouseButtonDown;
		OnMouseDblClickCallback OnMouseDblClick;

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
	{
	protected:
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseButtonUp& event) {}
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseButtonDown& event) {}
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseDblClick& event) {}
	};

	// Base class that listens to all input devices events
	class RYU_API InputEventListener 
		: public KeyboardEventListener
		, public MouseEventListener
	{

	};
}
#pragma	once
#include "Event/Event.h"
#include "Window/Input/KeyCode.h"

namespace Ryu::Window
{
	struct WindowEventBase: public Event::Event 
	{ 
		WindowEventBase() = default;
		WindowEventBase(HWND handle) : WindowHandle(handle) {}
		
		HWND WindowHandle{ nullptr };
	};

	// Define empty events
#define EMPTY_EVENT(Name) struct Name : public WindowEventBase { Name(HWND handle) : WindowEventBase(handle) {} };
	EMPTY_EVENT(CloseEvent);
	EMPTY_EVENT(MinimizeEvent);
	EMPTY_EVENT(MaximizeEvent);
	EMPTY_EVENT(RestoreEvent);
#undef EMPTY_EVENT

	struct MoveEvent : public WindowEventBase
	{
		explicit MoveEvent(HWND handle, i32 x, i32 y)
			: WindowEventBase(handle), X(x), Y(y) {}

		i32 X;
		i32 Y;
	};

	struct FocusEvent : public WindowEventBase
	{
		explicit FocusEvent(HWND handle, bool hasFocus)
			: WindowEventBase(handle), HasFocus(hasFocus) {}
		bool HasFocus = true;
	};

	struct ResizeEvent : public WindowEventBase
	{
		ResizeEvent(HWND handle, i32 width, i32 height)
			: WindowEventBase(handle), Width(width), Height(height) {}

		i32 Width  = 0;
		i32 Height = 0;
	};

	struct KeyEvent : public WindowEventBase
	{
		explicit KeyEvent(HWND handle, KeyCode keyCode, KeyState state, ModifierKeys modifiers, bool isRepeating)
			: WindowEventBase(handle), KeyCode(keyCode), State(state), Modifiers(modifiers), IsRepeating(isRepeating) {}

		KeyCode KeyCode        = KeyCode::None;
		KeyState State         = KeyState::Up;
		ModifierKeys Modifiers = ModifierKeys::None;
		bool IsRepeating       = false;
	};

	struct MouseButtonEvent : public WindowEventBase
	{
		explicit MouseButtonEvent(HWND handle, i32 x, i32 y, KeyCode button, KeyState state, ModifierKeys modifiers)
			: WindowEventBase(handle), X(x), Y(y), Button(button), State(state), Modifiers(modifiers) {}

		i32 X                  = 0;
		i32 Y                  = 0;
		KeyCode Button         = KeyCode::None;
		KeyState State         = KeyState::Up;
		ModifierKeys Modifiers = ModifierKeys::None;
	};

	struct MouseMoveEvent : public WindowEventBase
	{
		explicit MouseMoveEvent(HWND handle, i32 x, i32 y, ModifierKeys modifiers)
			: WindowEventBase(handle), X(x), Y(y), Modifiers(modifiers) {}

		i32 X                  = 0;
		i32 Y                  = 0;
		ModifierKeys Modifiers = ModifierKeys::None;
	};

	struct MouseMoveRawEvent : public WindowEventBase
	{
		explicit MouseMoveRawEvent(HWND handle, i32 x, i32 y, ModifierKeys modifiers)
			: WindowEventBase(handle), X(x), Y(y), Modifiers(modifiers) {}

		i32 X = 0;
		i32 Y = 0;
		ModifierKeys Modifiers = ModifierKeys::None;
	};

	struct MouseWheelEvent : public WindowEventBase
	{
		enum class WheelType { Vertical, Horizontal };

		explicit MouseWheelEvent(HWND handle, i32 x, i32 y, i32 delta, WheelType type, ModifierKeys modifiers)
			: WindowEventBase(handle), X(x), Y(y), Delta(delta), Type(type), Modifiers(modifiers) {}

		i32 X                  = 0;
		i32 Y                  = 0;
		i32 Delta              = 0;
		WheelType Type         = WheelType::Vertical;
		ModifierKeys Modifiers = ModifierKeys::None;
	};

	struct TextInputEvent : public WindowEventBase
	{
		explicit TextInputEvent(HWND handle, char32 unicodeChar) 
			: WindowEventBase(handle), UnicodeChar(unicodeChar) {}

		char32 UnicodeChar;

		constexpr char AsChar() const
		{
			return UnicodeChar <= 0x7F ? static_cast<char>(UnicodeChar) : '?';
		}
	};
}

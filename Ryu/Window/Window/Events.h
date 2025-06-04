#pragma	once
#include "Common/StandardTypes.h"
#include <Windows.h>
#include <variant>

namespace Ryu::Window
{
	struct CloseEvent    { HWND WindowHandle; };
	struct MinimizeEvent { HWND WindowHandle; };
	struct MaximizeEvent { HWND WindowHandle; };
	struct RestoreEvent  { HWND WindowHandle; };

	struct MoveEvent
	{
		HWND WindowHandle; 
		i32 X;
		i32 Y;
	};

	struct FocusEvent
	{
		HWND WindowHandle;
		bool HasFocus = true;
	};

	struct ResizeEvent
	{
		HWND WindowHandle;
		i32 Width = 0;
		i32 Height = 0;
	};

	struct KeyEvent
	{
		HWND WindowHandle;
		i32 KeyCode      = -1;
		bool IsRepeating = false;
		bool IsDown      = false;
	};

	struct MouseButtonEvent
	{
		HWND WindowHandle;
		i32 X = 0;
		i32 Y = 0;
		i32 Button = -1; // 0=left, 1=right, 2=middle
		bool IsDown = false;
	};

	struct MouseMoveEvent
	{
		HWND WindowHandle;
		i32 X = 0;
		i32 Y = 0;
	};
	
	struct MouseMoveRawEvent
	{
		HWND WindowHandle;
		i32 X = 0;
		i32 Y = 0;
	};


	struct MouseWheelEvent
	{
		enum class WheelType { Vertical, Horizontal };

		HWND WindowHandle;
		i32 X = 0;
		i32 Y = 0;
		i32 Delta = 0;
		WheelType Type = WheelType::Vertical;
	};

	struct TextInputEvent
	{
		char32 UnicodeChar;

		constexpr char AsChar() const
		{
			return UnicodeChar <= 0x7F ? static_cast<char>(UnicodeChar) : '?';
		}
	};

	using WindowEvent = std::variant<
		CloseEvent,
		MoveEvent,
		MinimizeEvent,
		MaximizeEvent,
		RestoreEvent,
		ResizeEvent,
		FocusEvent,
		KeyEvent,
		MouseButtonEvent,
		MouseMoveEvent,
		MouseMoveRawEvent,
		MouseWheelEvent,
		TextInputEvent
	>;

	template<typename T>
	concept EventListener = requires(T t, const WindowEvent& event)
	{
		{ t(event) } -> std::same_as<void>;
	};		
}

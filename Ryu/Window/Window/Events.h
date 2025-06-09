#pragma	once
#include "Common/Overload.h"
#include "Window/Input/KeyCode.h"
#include <variant>
#include <type_traits>
#include <utility>

namespace Ryu::Window
{
	struct CloseEvent { HWND WindowHandle; };
	struct MinimizeEvent { HWND WindowHandle; };
	struct MaximizeEvent { HWND WindowHandle; };
	struct RestoreEvent { HWND WindowHandle; };

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
		i32 Width  = 0;
		i32 Height = 0;
	};

	struct KeyEvent
	{
		HWND WindowHandle;
		KeyCode KeyCode        = KeyCode::None;
		KeyState State         = KeyState::Up;
		ModifierKeys Modifiers = ModifierKeys::None;
		bool IsRepeating       = false;
	};

	struct MouseButtonEvent
	{
		HWND WindowHandle;
		i32 X                  = 0;
		i32 Y                  = 0;
		KeyCode Button         = KeyCode::None;
		KeyState State         = KeyState::Up;
		ModifierKeys Modifiers = ModifierKeys::None;
	};

	struct MouseMoveEvent
	{
		HWND WindowHandle;
		i32 X                  = 0;
		i32 Y                  = 0;
		ModifierKeys Modifiers = ModifierKeys::None;
	};

	struct MouseMoveRawEvent
	{
		HWND WindowHandle;
		i32 X = 0;
		i32 Y = 0;
		ModifierKeys Modifiers = ModifierKeys::None;
	};

	struct MouseWheelEvent
	{
		enum class WheelType { Vertical, Horizontal };

		HWND WindowHandle;
		i32 X                  = 0;
		i32 Y                  = 0;
		i32 Delta              = 0;
		WheelType Type         = WheelType::Vertical;
		ModifierKeys Modifiers = ModifierKeys::None;
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
	concept EventListener = requires(T t, const WindowEvent & event)
	{
		{ t(event) } -> std::same_as<void>;
	};

	template <typename T>
	using EventType = std::decay_t<T>;

	// Function to check if two types are the same (useful for visiting variants)
	template <typename T, typename U>
	static constexpr bool IsEventType()
	{
		return std::is_same_v<T, U>;
	}

	// Function to check if variant holds a specific type
	template <typename T>
	static constexpr bool HasEventType(const WindowEvent& event)
	{
		return std::holds_alternative<T>(event);
	}

	// Alias for std::get
	template <typename T>
	static constexpr auto AsEventType(const WindowEvent& event)
	{
		return std::get<T>(event);
	}

	namespace Internal
	{
		struct DefaultEventHandler
		{
			constexpr void operator()(const auto&) const noexcept {}
		};
	}


	template<typename... T>
	struct WindowEventVisitor : public Common::Overload<T..., Internal::DefaultEventHandler>
	{
		constexpr WindowEventVisitor(T... visitors)
			: Common::Overload<T..., Internal::DefaultEventHandler>(visitors..., Internal::DefaultEventHandler{})
		{
		}
	};
}

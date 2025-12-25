#pragma once
#include <Windows.h>

namespace Ryu::Window
{
	enum class KeyState
	{
		Up,      // Key is not pressed
		Down,    // Key was just pressed this frame
		Held,    // Key is being held down
		Released // Key was just released this frame
	};

	enum class KeyCode : i32
	{
		None,

		Key0, Key1, Key2, Key3, Key4,
		Key5, Key6, Key7, Key8, Key9,

		A, B, C, D, E, F, G, H, I,
		J, K, L, M, N, O, P, Q, R,
		S, T, U, V, W, X, Y, Z,

		F1, F2, F3, F4, F5, F6, F7, F8, F9,
		F10, F11, F12, F13, F14, F15, F16, 
		F17, F18, F19, F20, F21, F22, F23, F24,

		Space, Enter, Escape, Tab, Backspace, Delete,
		Insert, Home, End, PageUp, PageDown,

		ArrowLeft, ArrowRight, ArrowUp, ArrowDown,

		LeftShift, RightShift, Shift,
		LeftControl, RightControl, Control,
		LeftAlt, RightAlt, Alt,
		LeftWindows, RightWindows,

		Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
		Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
		NumpadMultiply, NumpadAdd, NumpadSeperator,
		NumpadSubtract, NumpadDecimal, NumpadDivide,

		CapsLock, NumLock, ScrollLock, SemiColon,
		Comma, PlusOrEquals,  MinusOrUnderscore, Period, SlashOrQuestion,
		Grave, LeftBracket, RightBracket, Quote,

		MouseLeft, MouseRight, MouseMiddle,
		MouseX1, MouseX2
	};

	enum class ModifierKeys
	{
		None    = 0,
		Shift   = RYU_ENUM_BIT(0),
		Control = RYU_ENUM_BIT(1),
		Alt     = RYU_ENUM_BIT(2),
		Windows = RYU_ENUM_BIT(3)
	};
	RYU_ENUM_ENABLE_BITMASK_OPERATORS(ModifierKeys)

	inline bool HasModifierKey(ModifierKeys modifiers, ModifierKeys check)
	{
		return EnumHasAllFlags(modifiers, check);
	}

	static inline KeyCode VirtualKeyToKeyCode(i32 vk)
	{
		static const std::unordered_map<i32, KeyCode> VK_TO_KEYCODE =
		{
			{ '0', KeyCode::Key0 }, { '1', KeyCode::Key1 }, { '2', KeyCode::Key2 }, { '3', KeyCode::Key3 }, { '4', KeyCode::Key4 },
			{ '5', KeyCode::Key5 }, { '6', KeyCode::Key6 }, { '7', KeyCode::Key7 }, { '8', KeyCode::Key8 }, { '9', KeyCode::Key9 },

			{ 'A', KeyCode::A }, { 'B', KeyCode::B }, { 'C', KeyCode::C },
			{ 'D', KeyCode::D }, { 'E', KeyCode::E }, { 'F', KeyCode::F },
			{ 'G', KeyCode::G }, { 'H', KeyCode::H }, { 'I', KeyCode::I },
			{ 'J', KeyCode::J }, { 'K', KeyCode::K }, { 'L', KeyCode::L },
			{ 'M', KeyCode::M }, { 'N', KeyCode::N }, { 'O', KeyCode::O },
			{ 'P', KeyCode::P }, { 'Q', KeyCode::Q }, { 'R', KeyCode::R },
			{ 'S', KeyCode::S }, { 'T', KeyCode::T }, { 'U', KeyCode::U },
			{ 'V', KeyCode::V }, { 'W', KeyCode::W }, { 'X', KeyCode::X },
			{ 'Y', KeyCode::Y }, { 'Z', KeyCode::Z },

			{ VK_F1, KeyCode::F1 }, { VK_F2, KeyCode::F2 }, { VK_F3, KeyCode::F3 }, { VK_F4, KeyCode::F4 }, { VK_F5, KeyCode::F5 },
			{ VK_F6, KeyCode::F6 }, { VK_F7, KeyCode::F7 }, { VK_F8, KeyCode::F8 }, { VK_F9, KeyCode::F9 }, { VK_F10, KeyCode::F10 },
			{ VK_F11, KeyCode::F11 }, { VK_F12, KeyCode::F12 }, { VK_F13, KeyCode::F13 }, { VK_F14, KeyCode::F14 }, { VK_F15, KeyCode::F15 },
			{ VK_F16, KeyCode::F16 }, { VK_F17, KeyCode::F17 }, { VK_F18, KeyCode::F18 }, { VK_F19, KeyCode::F19 }, { VK_F20, KeyCode::F20 },
			{ VK_F21, KeyCode::F21 }, { VK_F22, KeyCode::F22 }, { VK_F23, KeyCode::F23 }, { VK_F24, KeyCode::F24 },

			{ VK_SPACE, KeyCode::Space }, { VK_RETURN, KeyCode::Enter }, { VK_ESCAPE, KeyCode::Escape }, { VK_TAB, KeyCode::Tab },
			{ VK_BACK, KeyCode::Backspace }, { VK_DELETE, KeyCode::Delete }, { VK_INSERT, KeyCode::Insert },
			{ VK_HOME, KeyCode::Home }, { VK_END, KeyCode::End }, { VK_PRIOR, KeyCode::PageUp }, { VK_NEXT, KeyCode::PageDown },

			{ VK_LEFT, KeyCode::ArrowLeft }, { VK_RIGHT, KeyCode::ArrowRight }, { VK_UP, KeyCode::ArrowUp }, { VK_DOWN, KeyCode::ArrowDown },

			{ VK_LSHIFT, KeyCode::LeftShift }, { VK_RSHIFT, KeyCode::RightShift }, { VK_SHIFT, KeyCode::Shift },
			{ VK_LCONTROL, KeyCode::LeftControl }, { VK_RCONTROL, KeyCode::RightControl }, { VK_CONTROL, KeyCode::Control },
			{ VK_LMENU, KeyCode::LeftAlt }, { VK_RMENU, KeyCode::RightAlt }, { VK_MENU, KeyCode::Alt },
			{ VK_LWIN, KeyCode::LeftWindows }, { VK_RWIN, KeyCode::RightWindows },

			{ VK_NUMPAD0, KeyCode::Numpad0 }, { VK_NUMPAD1, KeyCode::Numpad1 }, { VK_NUMPAD2, KeyCode::Numpad2 }, { VK_NUMPAD3, KeyCode::Numpad3 },
			{ VK_NUMPAD4, KeyCode::Numpad4 }, { VK_NUMPAD5, KeyCode::Numpad5 }, { VK_NUMPAD6, KeyCode::Numpad6 }, { VK_NUMPAD7, KeyCode::Numpad7 },
			{ VK_NUMPAD8, KeyCode::Numpad8 }, { VK_NUMPAD9, KeyCode::Numpad9 }, { VK_MULTIPLY, KeyCode::NumpadMultiply }, { VK_ADD, KeyCode::NumpadAdd },
			{ VK_SEPARATOR, KeyCode::NumpadSeperator }, { VK_SUBTRACT, KeyCode::NumpadSubtract },
			{ VK_DECIMAL, KeyCode::NumpadDecimal }, { VK_DIVIDE, KeyCode::NumpadDivide },

			{ VK_CAPITAL, KeyCode::CapsLock }, { VK_NUMLOCK, KeyCode::NumLock }, { VK_SCROLL, KeyCode::ScrollLock },
			{ VK_OEM_1, KeyCode::SemiColon }, { VK_OEM_COMMA, KeyCode::Comma }, { VK_OEM_PLUS, KeyCode::PlusOrEquals }, 
			{ VK_OEM_MINUS, KeyCode::MinusOrUnderscore }, { VK_OEM_PERIOD, KeyCode::Period }, { VK_OEM_2, KeyCode::SlashOrQuestion },
			{ VK_OEM_3, KeyCode::Grave }, { VK_OEM_4, KeyCode::LeftBracket }, { VK_OEM_6, KeyCode::RightBracket }, { VK_OEM_7, KeyCode::Quote },
		};

		auto it = VK_TO_KEYCODE.find(vk);
		return (it != VK_TO_KEYCODE.end()) ? it->second : KeyCode::None;
	}

	static inline KeyCode MouseVirtualKeyToKeyCode(i32 vk)
	{
		static const std::unordered_map<i32, KeyCode> VK_TO_KEYCODE
		{
			{ MK_LBUTTON, KeyCode::MouseLeft },
			{ MK_RBUTTON, KeyCode::MouseRight },
			{ MK_MBUTTON, KeyCode::MouseMiddle},
			{ MK_XBUTTON1, KeyCode::MouseX1 },
			{ MK_XBUTTON2, KeyCode::MouseX2 },
			{ MK_SHIFT, KeyCode::Shift },
			{ MK_CONTROL, KeyCode::Control },
		};

		auto it = VK_TO_KEYCODE.find(vk);
		return (it != VK_TO_KEYCODE.end()) ? it->second : KeyCode::None;
	}

	static inline ModifierKeys GetCurrentModifierKeys()
	{
		ModifierKeys modifiers{ ModifierKeys::None };

		auto IsKeyDown = [](auto key) { return ::GetAsyncKeyState(key) & 0x8000; };

		// Shift + LS + RS
		if (IsKeyDown(VK_SHIFT) || IsKeyDown(VK_LSHIFT) || IsKeyDown(VK_RSHIFT))
		{
			modifiers |= ModifierKeys::Shift;
		}

		// Ctrl + LC + RC
		if (IsKeyDown(VK_CONTROL) || IsKeyDown(VK_LCONTROL) || IsKeyDown(VK_RCONTROL))
		{
			modifiers |= ModifierKeys::Control;
		}

		// Alt + LA + RA
		if (IsKeyDown(VK_MENU) || IsKeyDown(VK_LMENU) || IsKeyDown(VK_RMENU))
		{
			modifiers |= ModifierKeys::Alt;
		}

		// Windows key
		if (IsKeyDown(VK_LWIN) || IsKeyDown(VK_RWIN))
		{
			modifiers |= ModifierKeys::Windows;
		}

		return modifiers;
	}
}

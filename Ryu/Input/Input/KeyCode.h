#pragma once
#include "Common/Common.h"
#include <expected>
#include <Windows.h>

namespace Ryu::Input
{
	/**
	 * @brief Enum class for all keycodes
	 * @details Maps to the Win32 virtual key codes. More info: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	 */
	TENUM()
	enum class KeyCode : u32
	{
		None               = 0x00,
		LeftMouseButton    = VK_LBUTTON,
		RightMouseButton   = VK_RBUTTON,
		MiddleMouseButton  = VK_MBUTTON,
		Extra1MouseButton  = VK_XBUTTON1,
		Extra2MouseButton  = VK_XBUTTON2,
		_Reserved01        = 0x07,
		Backspace          = VK_BACK,
		Tab                = VK_TAB,
		_Reserved02        = 0x0A,
		_Reserved03        = 0x0B,
		Clear              = VK_CLEAR,
		Return             = VK_RETURN,
		_Reserved04        = 0x0E,
		_Reserved05        = 0x0F,
		Shift              = VK_SHIFT,
		Ctrl               = VK_CONTROL,
		Alt                = VK_MENU,
		Pause              = VK_PAUSE,
		CapsLock           = VK_CAPITAL,
		_Reserved06        = VK_KANA,
		_Reserved07        = VK_HANGEUL,
		_Reserved08        = VK_HANGUL,
		_Reserved09        = VK_IME_ON,
		_Reserved10        = VK_JUNJA,
		_Reserved11        = VK_FINAL,
		_Reserved12        = VK_HANJA,
		_Reserved13        = VK_KANJI,
		_Reserved14        = VK_IME_OFF,
		Escape             = VK_ESCAPE,
		Convert            = VK_CONVERT,
		NonConvert         = VK_NONCONVERT,
		Accept             = VK_ACCEPT,
		ModeChange         = VK_MODECHANGE,
		Space              = VK_SPACE,
		PageUp             = VK_PRIOR,
		PageDown           = VK_NEXT,
		End                = VK_END,
		Home               = VK_HOME,
		Left               = VK_LEFT,
		Up                 = VK_UP,
		Right              = VK_RIGHT,
		Down               = VK_DOWN,
		Select             = VK_SELECT,
		Print              = VK_PRINT,
		Execute            = VK_EXECUTE,
		PrintScreen        = VK_SNAPSHOT,
		Insert             = VK_INSERT,
		Delete             = VK_DELETE,
		Help               = VK_HELP,
		Num0               = '0',
		Num1               = '1',
		Num2               = '2',
		Num3               = '3',
		Num4               = '4',
		Num5               = '5',
		Num6               = '6',
		Num7               = '7',
		Num8               = '8',
		Num9               = '9',
		_Reserved15        = 0x3A,
		_Reserved16        = 0x3B,
		_Reserved17        = 0x3C,
		_Reserved18        = 0x3D,
		_Reserved19        = 0x3E,
		_Reserved20        = 0x3F,
		_Reserved21        = 0x40,
		A                  = 'A',
		B                  = 'B',
		C                  = 'C',
		D                  = 'D',
		E                  = 'E',
		F                  = 'F',
		G                  = 'G',
		H                  = 'H',
		I                  = 'I',
		J                  = 'J',
		K                  = 'K',
		L                  = 'L',
		M                  = 'M',
		N                  = 'N',
		O                  = 'O',
		P                  = 'P',
		Q                  = 'Q',
		R                  = 'R',
		S                  = 'S',
		T                  = 'T',
		U                  = 'U',
		V                  = 'V',
		W                  = 'W',
		X                  = 'X',
		Y                  = 'Y',
		Z                  = 'Z',
		LeftWin            = VK_LWIN,
		RightWin           = VK_RWIN,
		Apps               = VK_APPS,
		_Reserved22        = 0x5E,
		Sleep              = VK_SLEEP,
		Numpad0            = VK_NUMPAD0,
		Numpad1            = VK_NUMPAD1,
		Numpad2            = VK_NUMPAD2,
		Numpad3            = VK_NUMPAD3,
		Numpad4            = VK_NUMPAD4,
		Numpad5            = VK_NUMPAD5,
		Numpad6            = VK_NUMPAD6,
		Numpad7            = VK_NUMPAD7,
		Numpad8            = VK_NUMPAD8,
		Numpad9            = VK_NUMPAD9,
		Multiply           = VK_MULTIPLY,
		Add                = VK_ADD,
		Separator          = VK_SEPARATOR,
		Subtract           = VK_SUBTRACT,
		Decimal            = VK_DECIMAL,
		Divide             = VK_DIVIDE,
		F1                 = VK_F1,
		F2                 = VK_F2,
		F3                 = VK_F3,
		F4                 = VK_F4,
		F5                 = VK_F5,
		F6                 = VK_F6,
		F7                 = VK_F7,
		F8                 = VK_F8,
		F9                 = VK_F9,
		F10                = VK_F10,
		F11                = VK_F11,
		F12                = VK_F12,
		F13                = VK_F13,
		F14                = VK_F14,
		F15                = VK_F15,
		F16                = VK_F16,
		F17                = VK_F17,
		F18                = VK_F18,
		F19                = VK_F19,
		F20                = VK_F20,
		F21                = VK_F21,
		F22                = VK_F22,
		F23                = VK_F23,
		F24                = VK_F24,
		_Reserved23        = 0x88,
		_Reserved24        = 0x89,
		_Reserved25        = 0x8A,
		_Reserved26        = 0x8B,
		_Reserved27        = 0x8C,
		_Reserved28        = 0x8D,
		_Reserved29        = 0x8E,
		_Reserved30        = 0x8F,
		_NavReserved01     = VK_NAVIGATION_VIEW,
		_NavReserved02     = VK_NAVIGATION_MENU,
		_NavReserved03     = VK_NAVIGATION_UP,
		_NavReserved04     = VK_NAVIGATION_DOWN,
		_NavReserved05     = VK_NAVIGATION_LEFT,
		_NavReserved06     = VK_NAVIGATION_RIGHT,
		_NavReserved07     = VK_NAVIGATION_ACCEPT,
		_NavReserved08     = VK_NAVIGATION_CANCEL,
		NumLock            = VK_NUMLOCK,
		ScrollLock         = VK_SCROLL,
		NumpadEqual        = VK_OEM_NEC_EQUAL,
		_Reserved31        = VK_OEM_FJ_JISHO,
		_Reserved32        = VK_OEM_FJ_MASSHOU,
		_Reserved33        = VK_OEM_FJ_TOUROKU,
		_Reserved34        = VK_OEM_FJ_LOYA,
		_Reserved35        = VK_OEM_FJ_ROYA,
		LeftShift          = VK_LSHIFT,
		RightShift         = VK_RSHIFT,
		LeftControl        = VK_LCONTROL,
		RightControl       = VK_RCONTROL,
		LeftMenu           = VK_LMENU,
		RightMenu          = VK_RMENU,
		_Reserved36        = VK_BROWSER_BACK,
		_Reserved37        = VK_BROWSER_FORWARD,
		_Reserved38        = VK_BROWSER_REFRESH,
		_Reserved39        = VK_BROWSER_STOP,
		_Reserved40        = VK_BROWSER_SEARCH,
		_Reserved41        = VK_BROWSER_FAVORITES,
		_Reserved42        = VK_BROWSER_HOME,
		_Reserved43        = VK_VOLUME_MUTE,
		_Reserved44        = VK_VOLUME_DOWN,
		_Reserved45        = VK_VOLUME_UP,
		_Reserved46        = VK_MEDIA_NEXT_TRACK,
		_Reserved47        = VK_MEDIA_PREV_TRACK,
		_Reserved48        = VK_MEDIA_STOP,
		_Reserved49        = VK_MEDIA_PLAY_PAUSE,
		_Reserved50        = VK_LAUNCH_MAIL,
		_Reserved51        = VK_LAUNCH_MEDIA_SELECT,
		_Reserved52        = VK_LAUNCH_APP1,
		_Reserved53        = VK_LAUNCH_APP2,
		_Reserved54        = 0xB8,
		_Reserved55        = 0xB9,
		SemiColon          = VK_OEM_1,
		Equals             = VK_OEM_PLUS,
		Comma              = VK_OEM_COMMA,
		Minus              = VK_OEM_MINUS,
		Period             = VK_OEM_PERIOD,
		Slash              = VK_OEM_2,
		Grave              = VK_OEM_3,
		_Reserved56        = 0xC1,
		_Reserved57        = 0xC2,
		OpenSquareBracket  = VK_OEM_4,
		BackSlash          = VK_OEM_5,
		CloseSquareBracket = VK_OEM_6,
		Quote              = VK_OEM_7,
	};

	/**
	 * @brief Enum class for all modifier keys
	 */
	TENUM()
	enum class ModifierKeys
	{
		None         = KeyCode::None,
		LeftShift    = KeyCode::LeftShift,
		RightShift   = KeyCode::RightShift,
		LeftControl  = KeyCode::LeftControl,
		RightControl = KeyCode::RightControl,
		LeftAlt      = KeyCode::LeftMenu,
		RightAlt     = KeyCode::RightMenu
	};

	/**
	 * @brief Enum class for all mouse buttons
	 */
	TENUM()
	enum class MouseButton
	{
		None         = KeyCode::None,
		LeftButton   = KeyCode::LeftMouseButton,
		RightButton  = KeyCode::RightMouseButton,
		MiddleButton = KeyCode::MiddleMouseButton,
		XButton1     = KeyCode::Extra1MouseButton,
		XButton2     = KeyCode::Extra2MouseButton
	};

	/**
	 * @brief Checks if the given `u32` is a valid KeyCode
	 * @param value `u32` Mapping to virtual key
	 * @return `KeyCode` if valid. False if invalid
	 */
	std::expected<KeyCode, bool> IsValidKeyCodeValue(u32 value);

	/**
	 * @brief Checks if the given `u32` is a valid `MouseButton`
	 * @param value `u32` Mapping to virtual key
	 * @return `MouseButton` if valid. False if invalid
	 */
	std::expected<MouseButton, bool> IsValidMouseButtonsValue(u32 value);
}

#include "KeyCode.gen.h"

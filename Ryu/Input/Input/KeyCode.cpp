#include "KeyCode.h"

namespace Ryu::Input
{
	std::expected<KeyCode, bool> IsValidKeyCodeValue(u32 value)
	{
		KeyCode keyCode = static_cast<KeyCode>(value);

		switch (keyCode)
		{
			using enum KeyCode;

		case LeftMouseButton:    FALLTHROUGH;
		case RightMouseButton:   FALLTHROUGH;
		case MiddleMouseButton:  FALLTHROUGH;
		case Extra1MouseButton:  FALLTHROUGH;
		case Extra2MouseButton:  FALLTHROUGH;
		case Backspace:          FALLTHROUGH;
		case Tab:                FALLTHROUGH;
		case Clear:              FALLTHROUGH;
		case Return:             FALLTHROUGH;
		case Shift:              FALLTHROUGH;
		case Ctrl:               FALLTHROUGH;
		case Alt:                FALLTHROUGH;
		case Pause:              FALLTHROUGH;
		case CapsLock:           FALLTHROUGH;
		case Escape:             FALLTHROUGH;
		case Convert:            FALLTHROUGH;
		case NonConvert:         FALLTHROUGH;
		case Accept:             FALLTHROUGH;
		case ModeChange:         FALLTHROUGH;
		case Space:              FALLTHROUGH;
		case PageUp:             FALLTHROUGH;
		case PageDown:           FALLTHROUGH;
		case End:                FALLTHROUGH;
		case Home:               FALLTHROUGH;
		case Left:               FALLTHROUGH;
		case Up:                 FALLTHROUGH;
		case Right:              FALLTHROUGH;
		case Down:               FALLTHROUGH;
		case Select:             FALLTHROUGH;
		case Print:              FALLTHROUGH;
		case Execute:            FALLTHROUGH;
		case PrintScreen:        FALLTHROUGH;
		case Insert:             FALLTHROUGH;
		case Delete:             FALLTHROUGH;
		case Help:               FALLTHROUGH;
		case Num0:               FALLTHROUGH;
		case Num1:               FALLTHROUGH;
		case Num2:               FALLTHROUGH;
		case Num3:               FALLTHROUGH;
		case Num4:               FALLTHROUGH;
		case Num5:               FALLTHROUGH;
		case Num6:               FALLTHROUGH;
		case Num7:               FALLTHROUGH;
		case Num8:               FALLTHROUGH;
		case Num9:               FALLTHROUGH;
		case A:                  FALLTHROUGH;
		case B:                  FALLTHROUGH;
		case C:                  FALLTHROUGH;
		case D:                  FALLTHROUGH;
		case E:                  FALLTHROUGH;
		case F:                  FALLTHROUGH;
		case G:                  FALLTHROUGH;
		case H:                  FALLTHROUGH;
		case I:                  FALLTHROUGH;
		case J:                  FALLTHROUGH;
		case K:                  FALLTHROUGH;
		case L:                  FALLTHROUGH;
		case M:                  FALLTHROUGH;
		case N:                  FALLTHROUGH;
		case O:                  FALLTHROUGH;
		case P:                  FALLTHROUGH;
		case Q:                  FALLTHROUGH;
		case R:                  FALLTHROUGH;
		case S:                  FALLTHROUGH;
		case T:                  FALLTHROUGH;
		case U:                  FALLTHROUGH;
		case V:                  FALLTHROUGH;
		case W:                  FALLTHROUGH;
		case X:                  FALLTHROUGH;
		case Y:                  FALLTHROUGH;
		case Z:                  FALLTHROUGH;
		case LeftWin:            FALLTHROUGH;
		case RightWin:           FALLTHROUGH;
		case Apps:               FALLTHROUGH;
		case Sleep:              FALLTHROUGH;
		case Numpad0:            FALLTHROUGH;
		case Numpad1:            FALLTHROUGH;
		case Numpad2:            FALLTHROUGH;
		case Numpad3:            FALLTHROUGH;
		case Numpad4:            FALLTHROUGH;
		case Numpad5:            FALLTHROUGH;
		case Numpad6:            FALLTHROUGH;
		case Numpad7:            FALLTHROUGH;
		case Numpad8:            FALLTHROUGH;
		case Numpad9:            FALLTHROUGH;
		case Multiply:           FALLTHROUGH;
		case Add:                FALLTHROUGH;
		case Separator:          FALLTHROUGH;
		case Subtract:           FALLTHROUGH;
		case Decimal:            FALLTHROUGH;
		case Divide:             FALLTHROUGH;
		case F1:                 FALLTHROUGH;
		case F2:                 FALLTHROUGH;
		case F3:                 FALLTHROUGH;
		case F4:                 FALLTHROUGH;
		case F5:                 FALLTHROUGH;
		case F6:                 FALLTHROUGH;
		case F7:                 FALLTHROUGH;
		case F8:                 FALLTHROUGH;
		case F9:                 FALLTHROUGH;
		case F10:                FALLTHROUGH;
		case F11:                FALLTHROUGH;
		case F12:                FALLTHROUGH;
		case F13:                FALLTHROUGH;
		case F14:                FALLTHROUGH;
		case F15:                FALLTHROUGH;
		case F16:                FALLTHROUGH;
		case F17:                FALLTHROUGH;
		case F18:                FALLTHROUGH;
		case F19:                FALLTHROUGH;
		case F20:                FALLTHROUGH;
		case F21:                FALLTHROUGH;
		case F22:                FALLTHROUGH;
		case F23:                FALLTHROUGH;
		case F24:                FALLTHROUGH;
		case NumLock:            FALLTHROUGH;
		case ScrollLock:         FALLTHROUGH;
		case NumpadEqual:        FALLTHROUGH;
		case LeftShift:          FALLTHROUGH;
		case RightShift:         FALLTHROUGH;
		case LeftControl:        FALLTHROUGH;
		case RightControl:       FALLTHROUGH;
		case LeftMenu:           FALLTHROUGH;
		case RightMenu:          FALLTHROUGH;
		case SemiColon:          FALLTHROUGH;
		case Equals:             FALLTHROUGH;
		case Comma:              FALLTHROUGH;
		case Minus:              FALLTHROUGH;
		case Period:             FALLTHROUGH;
		case Slash:              FALLTHROUGH;
		case Grave:              FALLTHROUGH;
		case OpenSquareBracket:  FALLTHROUGH;
		case BackSlash:          FALLTHROUGH;
		case CloseSquareBracket: FALLTHROUGH;
		case Quote:
			return keyCode;
		default:
			return std::unexpected(false);
		}
	}

	std::expected<MouseButton, bool> IsValidMouseButtonsValue(u32 value)
	{
		// First check if it is a valid KeyCode
		// Since keycodes include mouse buttons
		auto result = IsValidKeyCodeValue(value);
		if (!result)
		{
			return std::unexpected(false);
		}

		MouseButton btn = static_cast<MouseButton>(value);
		
		switch (btn)
		{
			using enum MouseButton;
		case LeftButton:   FALLTHROUGH;
		case RightButton:  FALLTHROUGH;
		case MiddleButton: FALLTHROUGH;
		case XButton1:     FALLTHROUGH;
		case XButton2:
			return btn;
		default:
			return std::unexpected(false);
		}
	}
}

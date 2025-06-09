#include "Window/Window.h"
#include "Profiling/Profiling.h"

/**
 * This file implements the Ryu::Window::Window::WWindowProc
 */

namespace Ryu::Window
{
	LRESULT Window::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		RYU_PROFILE_SCOPE();
		switch (msg)
		{
		case WM_CLOSE:
		{
			m_shouldClose = true;
			DispatchEvent(CloseEvent{ hwnd });
			break;
		}

		case WM_ENTERSIZEMOVE:
		{
			HandleResizeTracking();
			break;
		}

		case WM_EXITSIZEMOVE:
		{
			EndResizeTracking();
			break;
		}

		case WM_SIZE:
		{
			const i32 width  = LOWORD(lParam);
			const i32 height = HIWORD(lParam);
			m_config.Width   = width;
			m_config.Height  = height;
			m_currentSize    = { width, height };

			if (wParam == SIZE_MINIMIZED)
			{
				DispatchEvent(MinimizeEvent{ hwnd });
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				DispatchEvent(MaximizeEvent{ hwnd });
				if (m_currentSize != m_prevSize)
				{
					DispatchEvent(ResizeEvent{ hwnd, width, height });
					m_prevSize = m_currentSize;
				}
			}
			else if (wParam == SIZE_RESTORED && !m_isResizing)
			{
				DispatchEvent(RestoreEvent{ hwnd });
				if (m_currentSize != m_prevSize)
				{
					DispatchEvent(ResizeEvent{ hwnd, width, height });
					m_prevSize = m_currentSize;
				}
			}
			break;
		}

		case WM_MOVE:
		{
			const i32 xPos = (i32)(short)LOWORD(lParam);
			const i32 yPos = (i32)(short)HIWORD(lParam);

			m_config.X = xPos;
			m_config.Y = yPos;

			DispatchEvent(MoveEvent{ hwnd, xPos, yPos });
			break;
		}

		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			bool isDown            = (msg == WM_KEYDOWN);
			bool isRepeat          = (lParam & 0x40000000) != 0;
			KeyCode keyCode        = VirtualKeyToKeyCode(static_cast<i32>(wParam));
			ModifierKeys modifiers = GetCurrentModifierKeys();

			// Update input system state first
			m_input.SetKeyState(keyCode, isDown);
			m_input.SetModifierKeys(modifiers);

			// Determine the KeyState for the event
			KeyState eventState = KeyState::Up;
			if (isDown)
			{
				// For key down events, check if it's a repeat or new press
				eventState = isRepeat ? KeyState::Held : KeyState::Down;
			}
			else
			{
				// For key up events, it's always Released
				eventState = KeyState::Released;
			}

			DispatchEvent(KeyEvent{ hwnd, keyCode, eventState, modifiers, isRepeat });
			break;
		}

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		{
			const bool isDown = (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN);
			KeyCode button = KeyCode::None;

			if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
			{
				button = KeyCode::MouseLeft;
			}
			else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
			{
				button = KeyCode::MouseRight;
			}
			else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
			{
				button = KeyCode::MouseMiddle;
			}

			ModifierKeys modifiers = GetCurrentModifierKeys();

			// Update input system state first
			m_input.SetMouseButtonState(button, isDown);
			m_input.SetMousePosition(LOWORD(lParam), HIWORD(lParam));
			m_input.SetModifierKeys(modifiers);

			// Determine the KeyState for the event
			KeyState eventState = isDown ? KeyState::Down : KeyState::Released;

			DispatchEvent(MouseButtonEvent{ hwnd, LOWORD(lParam), HIWORD(lParam), button, eventState, modifiers });
			break;
		}

		case WM_MOUSEMOVE:
		{
			ModifierKeys modifiers = GetCurrentModifierKeys();

			m_input.SetMousePosition(LOWORD(lParam), HIWORD(lParam));
			m_input.SetModifierKeys(modifiers);

			DispatchEvent(MouseMoveEvent{ hwnd, LOWORD(lParam), HIWORD(lParam), modifiers });
			break;
		}

		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
		{
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			::ScreenToClient(hwnd, &pt);

			i32 delta = GET_WHEEL_DELTA_WPARAM(wParam) / 120;
			ModifierKeys modifiers = GetCurrentModifierKeys();

			m_input.SetModifierKeys(modifiers);

			DispatchEvent(
				MouseWheelEvent
				{
					hwnd, pt.x, pt.y, delta,
					(msg == WM_MOUSEWHEEL)
						? MouseWheelEvent::WheelType::Vertical
						: MouseWheelEvent::WheelType::Horizontal,
					modifiers
				});
			break;
		}

		case WM_CHAR:
		{
			char32 unicodeChar = static_cast<char32>(wParam);
			DispatchEvent(TextInputEvent{ unicodeChar });
			break;
		}

		case WM_INPUT:
		{
			UINT dwSize = 0;
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));

			if (dwSize > 0)
			{
				std::vector<BYTE> lpb(dwSize);
				if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb.data(), &dwSize, sizeof(RAWINPUTHEADER)) == dwSize)
				{
					RAWINPUT* raw = (RAWINPUT*)lpb.data();
					if (raw->header.dwType == RIM_TYPEMOUSE)
					{
						i32 deltaX = raw->data.mouse.lLastX;
						i32 deltaY = raw->data.mouse.lLastY;

						if (deltaX != 0 || deltaY != 0)
						{
							ModifierKeys modifiers = GetCurrentModifierKeys();

							m_input.SetMouseDelta(deltaX, deltaY);
							m_input.SetModifierKeys(modifiers);

							DispatchEvent(MouseMoveRawEvent{ hwnd, deltaX, deltaY, modifiers });
						}
					}
				}
			}
			break;
		}
		}

		return ::DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}

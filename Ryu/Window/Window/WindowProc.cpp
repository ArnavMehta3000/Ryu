#include "Window/Window.h"

/**
 * This file implements:
 * - Ryu::Window::Window::WindowProc
 * - Ryu::Window::Window::HitTest()
 */

 // Ref: https://github.com/melak47/BorderlessWindow

#if defined(RYU_WITH_EDITOR)
#include <External/ImGui/backends/imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

namespace Ryu::Window
{
	LRESULT Window::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
#if defined(RYU_WITH_EDITOR)
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		{
			return true;
		}
#endif

		switch (msg)
		{
		case WM_CLOSE:
		{
			m_shouldClose = true;
			Emit(CloseEvent(hwnd));
			break;
		}

		case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = static_cast<LONG>(m_config.WindowMinSize.X);
			lpMMI->ptMinTrackSize.y = static_cast<LONG>(m_config.WindowMinSize.Y);
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
			const i32 width     = LOWORD(lParam);
			const i32 height    = HIWORD(lParam);
			m_config.WindowSize = { width, height };
			m_currentSize       = { width, height };

			if (wParam == SIZE_MINIMIZED)
			{
				Emit(MinimizeEvent(hwnd));
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				Emit(MaximizeEvent(hwnd));
				if (m_currentSize != m_prevSize)
				{
					Emit(ResizeEvent(hwnd, width, height));
					m_prevSize = m_currentSize;
				}
			}
			else if (wParam == SIZE_RESTORED && !m_isResizing)
			{
				Emit(RestoreEvent(hwnd));
				if (m_currentSize != m_prevSize)
				{
					Emit(ResizeEvent(hwnd, width, height));
					m_prevSize = m_currentSize;
				}
			}
			break;
		}

		case WM_MOVE:
		{
			const i32 xPos = (i32)(short)LOWORD(lParam);
			const i32 yPos = (i32)(short)HIWORD(lParam);

			m_config.WindowPos = { xPos, yPos };

			Emit(MoveEvent(hwnd, xPos, yPos ));
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

			Emit(KeyEvent(hwnd, keyCode, eventState, modifiers, isRepeat));
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

			Emit(MouseButtonEvent(hwnd, LOWORD(lParam), HIWORD(lParam), button, eventState, modifiers));
			break;
		}

		case WM_MOUSEMOVE:
		{
			ModifierKeys modifiers = GetCurrentModifierKeys();

			m_input.SetMousePosition(LOWORD(lParam), HIWORD(lParam));
			m_input.SetModifierKeys(modifiers);

			Emit(MouseMoveEvent(hwnd, LOWORD(lParam), HIWORD(lParam), modifiers));
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

			Emit(
				MouseWheelEvent
				(
					hwnd,
					pt.x, pt.y,
					delta,
					(msg == WM_MOUSEWHEEL)
						? MouseWheelEvent::WheelType::Vertical
						: MouseWheelEvent::WheelType::Horizontal,
					modifiers
				));
			break;
		}

		case WM_CHAR:
		{
			char32 unicodeChar = static_cast<char32>(wParam);
			Emit(TextInputEvent(hwnd, unicodeChar));
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

							Emit(MouseMoveRawEvent(hwnd, deltaX, deltaY, modifiers));
						}
					}
				}
			}
			break;
		}

		case WM_NCCALCSIZE:
		{
			if (wParam == TRUE && m_config.Type == WindowType::Borderless)
			{
				NCCALCSIZE_PARAMS* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
				
				// Adjust maximized client rect
				if (IsMaximized())
				{
					HMONITOR monitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
					if (monitor)
					{
						MONITORINFO monitorInfo{ .cbSize = sizeof(MONITORINFO) };
						if (::GetMonitorInfo(monitor, &monitorInfo))
						{
							params->rgrc[0] = monitorInfo.rcWork;
						}
					}
				}
				
				// ALWAYS return 0 for borderless windows to remove non-client area
				return 0;
			}

			break;
		}

		case WM_NCHITTEST:
		{
			// When we have no border or title bar, we need to perform our
			// own hit testing to allow resizing and moving.
			if (m_config.Type == WindowType::Borderless)
			{
				return HitTest({ LOWORD(lParam), HIWORD(lParam) });
			}

			break;
		}

		case WM_NCACTIVATE:
		{
			if (!IsCompositionEnabled())
			{
				// Prevents window frame reappearing on window activation
				// in "basic" theme, where no aero shadow is present.
				return 1;
			}
			break;
		}
		}

		return ::DefWindowProcW(hwnd, msg, wParam, lParam);
	}

	LRESULT Window::HitTest(POINT cursor) const
	{
		const POINT border
		{
			::GetSystemMetrics(SM_CXFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER),
        	::GetSystemMetrics(SM_CYFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER)
		};

		RECT window{};
		if (!::GetWindowRect(m_hwnd, &window))
		{
			return HTNOWHERE;
		}

		const LRESULT drag = m_config.CanBorderlessDrag ? HTCAPTION : HTCLIENT;
		const bool borderlessResize = m_config.CanBorderlessResize;

		enum RegionMask
		{
			Client = 0b0000,
			Left   = 0b0001,
			Right  = 0b0010,
			Top    = 0b0100,
			Bottom = 0b1000,
		};

		const i32 result = Left * (cursor.x <  (window.left + border.x)) 
			| Right  * (cursor.x >= (window.right - border.x))
			| Top    * (cursor.y <  (window.top + border.y))
			| Bottom * (cursor.y >= (window.bottom - border.y));

		

		switch (result)
		{
		case Left          : return borderlessResize ? HTLEFT        : drag;
		case Right         : return borderlessResize ? HTRIGHT       : drag;
		case Top           : return borderlessResize ? HTTOP         : drag;
		case Bottom        : return borderlessResize ? HTBOTTOM      : drag;
		case Top | Left    : return borderlessResize ? HTTOPLEFT     : drag;
		case Top | Right   : return borderlessResize ? HTTOPRIGHT    : drag;
		case Bottom | Left : return borderlessResize ? HTBOTTOMLEFT  : drag;
		case Bottom | Right: return borderlessResize ? HTBOTTOMRIGHT : drag;
		case Client        : return drag;
		default            : return HTNOWHERE;
		}
	}
}

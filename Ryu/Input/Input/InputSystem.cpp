#include "InputSystem.h"

namespace Ryu::Input
{
	namespace
	{
		InputSystem* g_instance = nullptr;
	}

	InputSystem::InputSystem(HWND window, IInputEventListener* eventListener)
		: m_originalWndProc(nullptr)
		, m_eventListener(eventListener)
		, m_keyboard()
		, m_mouse()
		, m_hWnd(window)
	{
		g_instance = this;

		m_keyboard.Create(eventListener);
		m_mouse.Create(m_hWnd, eventListener);

		m_originalWndProc = (WNDPROC)::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)&InputSystem::InputWndProc);
	}

	InputSystem::~InputSystem()
	{
		// Restore original WndProc
		::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_originalWndProc);
	}

	LRESULT InputSystem::InputWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (!g_instance)
		{
			return ::DefWindowProc(hWnd, msg, wParam, lParam);
		}


		switch (msg)
		{
		// Keyboard messages
		case WM_KEYDOWN:    FALLTHROUGH;
		case WM_SYSKEYDOWN: FALLTHROUGH;
		case WM_KEYUP:      FALLTHROUGH;
		case WM_SYSKEYUP:
		{
			g_instance->m_keyboard.Read(wParam, lParam);
			break;
		}

		// Mouse messages
		case WM_MOUSEACTIVATE:
			return MA_ACTIVATEANDEAT;

		case WM_LBUTTONDOWN:
		{
			g_instance->m_mouse.OnClick(MouseButton::LeftButton, lParam, true);
			break;
		}

		case WM_RBUTTONDOWN:
		{
			g_instance->m_mouse.OnClick(MouseButton::RightButton, lParam, true);
			break;
		}
		case WM_MBUTTONDOWN:
		{
			g_instance->m_mouse.OnClick(MouseButton::MiddleButton, lParam, true);
			break;
		}
		case WM_XBUTTONDOWN:
		{
			g_instance->m_mouse.OnClick(HIWORD(wParam) == XBUTTON1 ? 
				MouseButton::XButton1 : MouseButton::XButton2, lParam, true);
			break;
		}
		case WM_LBUTTONUP:
		{
			g_instance->m_mouse.OnClick(MouseButton::LeftButton, lParam, false);
			break;
		}
		case WM_RBUTTONUP:
		{
			g_instance->m_mouse.OnClick(MouseButton::RightButton, lParam, false);
			break;
		}
		case WM_MBUTTONUP:
		{
			g_instance->m_mouse.OnClick(MouseButton::MiddleButton, lParam, false);
			break;
		}
		case WM_XBUTTONUP:
		{
			g_instance->m_mouse.OnClick(HIWORD(wParam) == XBUTTON1 ? 
				MouseButton::XButton1 : MouseButton::XButton2, lParam, false);
			break;
		}
		case WM_LBUTTONDBLCLK:
		{
			g_instance->m_mouse.OnDblClick(MouseButton::LeftButton, wParam, lParam);
			break;
		}
		case WM_RBUTTONDBLCLK:
		{
			g_instance->m_mouse.OnDblClick(MouseButton::RightButton, wParam, lParam);
			break;
		}
		case WM_MBUTTONDBLCLK:
		{
			g_instance->m_mouse.OnDblClick(MouseButton::MiddleButton, wParam, lParam);
			break;
		}
		case WM_XBUTTONDBLCLK:
		{
			g_instance->m_mouse.OnDblClick(HIWORD(wParam) == XBUTTON1 ?
				MouseButton::XButton1 : MouseButton::XButton2, wParam, lParam);
			break;
		}
		case WM_MOUSEMOVE:
		{
			g_instance->m_mouse.OnMove(wParam, lParam);
			break;
		}
		case WM_INPUT:
		{
			g_instance->m_mouse.OnMoveRaw(wParam, lParam);
			break;
		}


		case WM_MOUSEWHEEL:
		{
			g_instance->m_mouse.OnWheel(wParam, lParam);
			break;
		}
		}

		return ::CallWindowProc(g_instance->m_originalWndProc, hWnd, msg, wParam, lParam);
	}
}

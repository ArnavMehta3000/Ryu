#include "Mouse.h"
#include "Input/Events/MouseEvents.h"
#include <windowsx.h>

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((unsigned short) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((unsigned short) 0x02)
#endif

namespace Ryu::Input
{
	namespace
	{
		MouseButton GetMouseButton(WPARAM wParam)
		{
			if (wParam & MK_LBUTTON)
				return MouseButton::LeftButton;

			if (wParam & MK_RBUTTON)
				return MouseButton::RightButton;

			if (wParam & MK_MBUTTON)
				return MouseButton::MiddleButton;

			if (wParam & MK_XBUTTON1)
				return MouseButton::XButton1;

			if (wParam & MK_XBUTTON2)
				return MouseButton::XButton2;

			return MouseButton::None;
		}

		void GetMousePosition(LPARAM lParam, i32& x, i32& y)
		{
			x = GET_X_LPARAM(lParam);
			y = GET_Y_LPARAM(lParam);
		}
	}

	void Mouse::Create(HWND window, IMouseEventListener* eventListener)
	{
		m_eventListener = eventListener;

		// Create raw mouse input
		RAWINPUTDEVICE rid[1];
		rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		rid[0].usUsage     = HID_USAGE_GENERIC_MOUSE;
		rid[0].dwFlags     = RIDEV_INPUTSINK;
		rid[0].hwndTarget  = window;
		RegisterRawInputDevices(rid, 1, sizeof(rid[0]));
	}

	void Mouse::OnClick(MouseButton button, LPARAM lParam, bool isDown)
	{
		if (button == MouseButton::None)
		{
			return;
		}

		// TODO: Check for modifier keys

		i32 x = 0, y = 0;
		GetMousePosition(lParam, x, y);
		m_posX = x;
		m_posY = y;

		// Fire callbacks
		if (isDown)
		{
			m_eventListener->OnEvent(Events::OnMouseButtonDown(button, x, y));
		}
		else
		{
			m_eventListener->OnEvent(Events::OnMouseButtonUp(button, x, y));
		}

	}

	void Mouse::OnDblClick(MouseButton button, MAYBE_UNUSED WPARAM wParam, LPARAM lParam)
	{
		if (button == MouseButton::None)
		{
			return;
		}

		// TODO: Check for modifier keys

		i32 x = 0, y = 0;
		GetMousePosition(lParam, x, y);
		m_posX = x;
		m_posY = y;

		// Fire callbacks
		m_eventListener->OnEvent(Events::OnMouseDblClick(button, x, y));
	}

	void Mouse::OnMove(WPARAM wParam, LPARAM lParam)
	{
		// Button can be none here
		MouseButton button = GetMouseButton(wParam);

		// TODO: Check for modifier keys

		i32 x = 0, y = 0;
		GetMousePosition(lParam, x, y);

		m_posX = x;
		m_posY = y;

		m_eventListener->OnEvent(Events::OnMouseMove(button, x, y));
	}

	void Mouse::OnMoveRaw(WPARAM wParam, LPARAM lParam)
	{
		// Button can be none here
		MouseButton button = GetMouseButton(wParam);

		// TODO: Check for modifier keys

		i32 x = 0, y = 0;
		GetMousePosition(lParam, x, y);

		unsigned size = sizeof(RAWINPUT);
		static RAWINPUT raw[sizeof(RAWINPUT)];
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, raw, &size, sizeof(RAWINPUTHEADER));

		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			i32 deltaX = raw->data.mouse.lLastX;
			i32 deltaY = raw->data.mouse.lLastY;
			m_eventListener->OnEvent(Events::OnMouseMoveRaw(button, deltaX, deltaY));
		}

	}

	void Mouse::OnWheel(WPARAM wParam, LPARAM lParam)
	{
		// Button can be none here
		MouseButton button = GetMouseButton(wParam);

		// TODO: Check for modifier keys

		i32 x = 0, y = 0;
		GetMousePosition(lParam, x, y);

		// Normalize delta
		const i32 delta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		
		m_eventListener->OnEvent(Events::OnMouseWheel(button, delta, x, y));
	}
}

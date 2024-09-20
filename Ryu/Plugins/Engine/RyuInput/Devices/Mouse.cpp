#include "Mouse.h"
#include <Plugins/Engine/RyuInput/Generated/RyuInputLog.h>
#include <RyuInput/InputEvents.h>
#include <windowsx.h>

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

	void Mouse::Create(std::vector<InputCallbacks>* callbacks)
	{
		m_callbacks = callbacks;
	}

	void Mouse::OnClick(MouseButton button, LPARAM lParam, bool isDown)
	{
		if (button == MouseButton::None)
		{
			PLUGIN_ERROR("Unknown mouse (OnClick) button: {} (isDown: {})", (u32)button, isDown);
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
			for (auto& callback : *m_callbacks)
			{
				if (callback.OnMouseButtonDown)
				{
					callback.OnMouseButtonDown(Events::OnMouseButtonDown(button, x, y));
				}
			}
		}
		else
		{
			for (auto& callback : *m_callbacks)
			{
				if (callback.OnMouseButtonUp)
				{
					callback.OnMouseButtonUp(Events::OnMouseButtonUp(button, x, y));
				}
			}
		}

	}

	void Mouse::OnDblClick(MouseButton button, MAYBE_UNUSED WPARAM wParam, LPARAM lParam)
	{
		if (button == MouseButton::None)
		{
			PLUGIN_ERROR("Unknown mouse (OnDblClick) button: {}", (u32)button);
			return;
		}

		// TODO: Check for modifier keys

		i32 x = 0, y = 0;
		GetMousePosition(lParam, x, y);
		m_posX = x;
		m_posY = y;

		// Fire callbacks
		for (auto& callback : *m_callbacks)
		{
			if (callback.OnMouseDblClick)
			{
				callback.OnMouseDblClick(Events::OnMouseDblClick(button, x, y));
			}
		}
	}

	void Mouse::OnMove(WPARAM wParam, LPARAM lParam)
	{
		// Button can be none here
		MouseButton button = GetMouseButton(wParam);

		// TODO: Check for modifier keys

		i32 x = 0, y = 0;
		GetMousePosition(lParam, x, y);

		// Calculate raw delta
		// NOTE: This is not real raw mouse movement, it's just a delta
		// between the current position and the previous position
		// Better to change it to use WM_INPUT for raw data
		const i32 deltaX = x - m_posX;
		const i32 deltaY = y - m_posY;

		m_posX = x;
		m_posY = y;

		// Fire callbacks
		for (auto& callback : *m_callbacks)
		{
			if (callback.OnMouseMove)
			{
				callback.OnMouseMove(Events::OnMouseMove(button, x, y));
			}

			if (callback.OnMouseMoveRaw)
			{
				callback.OnMouseMoveRaw(Events::OnMouseMoveRaw(button, deltaX, deltaY));
			}
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
		
		for (auto& callback : *m_callbacks)
		{
			if (callback.OnMouseWheel)
			{
				callback.OnMouseWheel(Events::OnMouseWheel(button, delta, x, y));
			}
		}
	}
}

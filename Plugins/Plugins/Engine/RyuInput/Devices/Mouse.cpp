#include "Mouse.h"
#include <Plugins/Engine/RyuInput/Internal/Log.h>
#include <Plugins/Engine/RyuInput/InputEvents.h>
#include <windowsx.h>

namespace Ryu::Input
{
	namespace
	{
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
			RYU_PLUGIN_ERROR("Unknown mouse (OnClick) button: {} (isDown: {})", (u32)button, isDown);
			return;
		}

		// TODO: Check for modifier keys

		i32 x = 0, y = 0;
		GetMousePosition(lParam, x, y);

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

	void Mouse::OnDblClick(MouseButton button, WPARAM wParam, LPARAM lParam)
	{
		if (button == MouseButton::None)
		{
			RYU_PLUGIN_ERROR("Unknown mouse (OnDblClick) button: {}", (u32)button);
			return;
		}

		// TODO: Check for modifier keys

		i32 x = 0, y = 0;
		GetMousePosition(lParam, x, y);

		// Fire callbacks
		for (auto& callback : *m_callbacks)
		{
			if (callback.OnMouseDblClick)
			{
				callback.OnMouseDblClick(Events::OnMouseDblClick(button, x, y));
			}
		}
	}

}
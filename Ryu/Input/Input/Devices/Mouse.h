#pragma once
#include "Input/KeyCode.h"
#include <unordered_map>

namespace Ryu::Input
{
	class IMouseEventListener;

	class Mouse
	{
		friend class InputSystem;

	private:
		void Create(HWND window, IMouseEventListener* eventListener);
		// Handles up and down events
		void OnClick(MouseButton button, LPARAM lParam, bool isDown);
		// Handles double click events
		void OnDblClick(MouseButton button, WPARAM wParam, LPARAM lParam);
		// Handles mouse move events
		void OnMove(WPARAM wParam, LPARAM lParam);
		void OnMoveRaw(WPARAM wParam, LPARAM lParam);
		void OnWheel(WPARAM wParam, LPARAM lParam);

	private:
		IMouseEventListener* m_eventListener;
		i32 m_posX;
		i32 m_posY;
	};
}

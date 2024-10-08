#pragma once
#include "Input/Events/InputEventListeners.h"
#include "Input/Devices/Keyboard.h"
#include "Input/Devices/Mouse.h"

namespace Ryu::Input
{
	class InputSystem
	{
	public:
		InputSystem(HWND window, IInputEventListener* eventListener);
		virtual ~InputSystem();
		static LRESULT CALLBACK InputWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
	private:
		WNDPROC              m_originalWndProc;
		IInputEventListener* m_eventListener;
		Keyboard             m_keyboard;
		Mouse                m_mouse;
		HWND                 m_hWnd;
	};
}

#pragma once
#include "Input/Events/InputEventListeners.h"
#include "Input/Devices/Keyboard.h"
#include "Input/Devices/Mouse.h"

namespace Ryu::Input
{
	/**
	 * @brief Class to handle all input events
	 * @details This class injects its own WndProc into the window and handles all input events. Then forwards the remaining calls to the original WndProc
	 */
	class InputSystem
	{
	public:
		/**
		 * @brief Constructs a new Input System
		 * @param window The window to inject the WndProc and listen for inputs
		 * @param eventListener The event listener (cannot be null)
		 */
		InputSystem(HWND window, IInputEventListener* eventListener);

		virtual ~InputSystem();

		/**
		 * @brief Handles all input events and forwards the remaining calls to the original WndProc
		 * @param hWnd Handle to the window
		 * @param msg Window message
		 * @param wParam WPARAM parameter
		 * @param lParam LPARAM parameter
		 * @return 
		 */
		static LRESULT CALLBACK InputWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
	private:
		WNDPROC              m_originalWndProc;
		IInputEventListener* m_eventListener;
		Keyboard             m_keyboard;
		Mouse                m_mouse;
		HWND                 m_hWnd;
	};
}

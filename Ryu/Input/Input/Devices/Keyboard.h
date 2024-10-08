#pragma once
#include "Input/KeyCode.h"
#include <unordered_map>

namespace Ryu::Input
{
	class IKeyboardEventListener;

	class Keyboard
	{
		friend class InputSystem;
	private:
		struct KeyState
		{
			bool IsDown = false;
			bool WasDown = false;
		};

		using KeyMap = std::unordered_map<KeyCode, KeyState>;

	private:
		void Create(IKeyboardEventListener* eventListener);
		void Read(WPARAM wParam, LPARAM lParam);
		void UpdateKeyState(KeyCode keyCode, bool isKeyReleased, bool wasKeyDown);
		void OnKeyUp(KeyCode key);
		void OnKeyDown(KeyCode key);

	private:
		KeyMap m_keyStates;
		IKeyboardEventListener* m_eventListener;
	};
}

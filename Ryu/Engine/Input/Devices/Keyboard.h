#pragma once
#include <Engine/Input/KeyCode.h>
#include <Engine/Input/InputEvents.h>
#include <unordered_map>

namespace Ryu
{
	class RYU_API Keyboard
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
		void Create(std::vector<InputCallbacks>* callbacks);
		void Read(WPARAM wParam, LPARAM lParam);
		void UpdateKeyState(KeyCode keyCode, bool isKeyReleased, bool wasKeyDown);
		void OnKeyUp(KeyCode key);
		void OnKeyDown(KeyCode key);

	private:
		KeyMap m_keyStates;
		std::vector<InputCallbacks>* m_callbacks;
	};
}

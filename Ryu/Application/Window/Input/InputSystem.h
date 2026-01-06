#pragma once
#include "Application/Window/WindowEvents.h"

namespace Ryu::Window
{
	class Window;

	class InputSystem
	{
		friend class Window;
	public:
		InputSystem() = default;
		~InputSystem() = default;

		bool IsKeyDown(KeyCode key) const;
		bool IsKeyUp(KeyCode key) const;
		bool IsKeyPressed(KeyCode key) const;  // Just pressed this frame
		bool IsKeyReleased(KeyCode key) const; // Just released this frame
		KeyState GetKeyState(KeyCode key) const;

		bool IsMouseButtonDown(KeyCode button) const;
		bool IsMouseButtonUp(KeyCode button) const;
		bool IsMouseButtonPressed(KeyCode button) const;
		bool IsMouseButtonReleased(KeyCode button) const;
		KeyState GetMouseButtonState(KeyCode button) const;

		inline void GetMousePosition(i32& x, i32& y) const { x = m_mouseX; y = m_mouseY; }
		inline void GetMouseDelta(i32& deltaX, i32& deltaY) const { deltaX = m_mouseDeltaX; deltaY = m_mouseDeltaY; }
		inline ModifierKeys GetModifierKeys() const { return m_currentModifiers; }
		bool HasModifier(ModifierKeys modifier) const;

		void EnableRawMouseInput(bool enable) const;
		inline bool IsRawMouseInputEnabled() const { return m_rawMouseEnabled; }

	private:
		void Initialize(HWND windowHandle);
		void Shutdown();
		void UpdateStates();
		KeyState GetStateForKey(KeyCode key, const std::unordered_map<KeyCode, KeyState>& stateMap) const;
		void UpdateKeyState(KeyCode key, bool isDown, std::unordered_map<KeyCode, KeyState>& stateMap);

		void SetKeyState(KeyCode key, bool isDown);
		void SetMouseButtonState(KeyCode button, bool isDown);
		void SetMousePosition(i32 x, i32 y);
		void SetMouseDelta(i32 deltaX, i32 deltaY);
		void SetModifierKeys(ModifierKeys modifiers);

	private:
		std::unordered_map<KeyCode, KeyState> m_keyStates;
		std::unordered_map<KeyCode, KeyState> m_mouseButtonStates;
		i32 m_mouseX = 0;
		i32 m_mouseY = 0;
		i32 m_mouseDeltaX = 0;
		i32 m_mouseDeltaY = 0;
		ModifierKeys m_currentModifiers = ModifierKeys::None;
		mutable bool m_rawMouseEnabled = false;
		HWND m_windowHandle = nullptr;
	};
}

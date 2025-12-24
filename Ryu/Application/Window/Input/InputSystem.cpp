#include "Application/Window/Input/InputSystem.h"

namespace Ryu::Window
{
	bool InputSystem::IsKeyDown(KeyCode key) const
	{
		KeyState state = GetKeyState(key);
		return state == KeyState::Down || state == KeyState::Held;
	}
	
	bool InputSystem::IsKeyUp(KeyCode key) const
	{
		KeyState state = GetKeyState(key);
		return state == KeyState::Up || state == KeyState::Released;
	}
	
	bool InputSystem::IsKeyPressed(KeyCode key) const
	{
		return GetKeyState(key) == KeyState::Down;
	}
	
	bool InputSystem::IsKeyReleased(KeyCode key) const
	{
		return GetKeyState(key) == KeyState::Released;
	}
	
	KeyState InputSystem::GetKeyState(KeyCode key) const
	{
		return GetStateForKey(key, m_keyStates);
	}
	
	bool InputSystem::IsMouseButtonDown(KeyCode button) const
	{
		KeyState state = GetMouseButtonState(button);
		return state == KeyState::Down || state == KeyState::Held;
	}
	
	bool InputSystem::IsMouseButtonUp(KeyCode button) const
	{
		KeyState state = GetMouseButtonState(button);
		return state == KeyState::Up || state == KeyState::Released;
	}
	
	bool InputSystem::IsMouseButtonPressed(KeyCode button) const
	{
		return GetMouseButtonState(button) == KeyState::Down;
	}
	
	bool InputSystem::IsMouseButtonReleased(KeyCode button) const
	{
		return GetMouseButtonState(button) == KeyState::Released;
	}
	
	KeyState InputSystem::GetMouseButtonState(KeyCode button) const
	{
		return GetStateForKey(button, m_mouseButtonStates);
	}

	bool InputSystem::HasModifier(ModifierKeys modifier) const
	{
		return HasModifierKey(m_currentModifiers, modifier);
	}

	void InputSystem::EnableRawMouseInput(bool enable) const
	{
		if (enable == m_rawMouseEnabled || !m_windowHandle)
		{
			return;
		}

		if (enable)
		{
			RAWINPUTDEVICE rid{};
			rid.usUsagePage = 0x01;  // Generic desktop
			rid.usUsage     = 0x02;      // Mouse
			rid.dwFlags     = 0;
			rid.hwndTarget  = m_windowHandle;

			if (::RegisterRawInputDevices(&rid, 1, sizeof(rid)))
			{
				m_rawMouseEnabled = true;
			}
		}
		else
		{
			RAWINPUTDEVICE rid{};
			rid.usUsagePage = 0x01;
			rid.usUsage = 0x02;
			rid.dwFlags = RIDEV_REMOVE;
			rid.hwndTarget = nullptr;

			if (::RegisterRawInputDevices(&rid, 1, sizeof(rid)))
			{
				m_rawMouseEnabled = false;
			}
		}
	}
	
	void InputSystem::Initialize(HWND windowHandle)
	{
		m_windowHandle = windowHandle;

		// Get initial mouse position
		POINT pt{};
		if (::GetCursorPos(&pt) && ::ScreenToClient(m_windowHandle, &pt))
		{
			m_mouseX = pt.x;
			m_mouseY = pt.y;
		}

		m_mouseDeltaX = m_mouseDeltaY = 0;
	}

	void InputSystem::Shutdown()
	{
		EnableRawMouseInput(false);
		m_keyStates.clear();
		m_mouseButtonStates.clear();
	}
	
	void InputSystem::UpdateStates()
	{
		// Convert Down states to Held, Released states to Up
		for (auto& [key, state] : m_keyStates)
		{
			if (state == KeyState::Down)
			{
				state = KeyState::Held;
			}
			else if (state == KeyState::Released)
			{
				state = KeyState::Up;
			}
		}

		for (auto& [button, state] : m_mouseButtonStates)
		{
			if (state == KeyState::Down)
			{
				state = KeyState::Held;
			}
			else if (state == KeyState::Released)
			{
				state = KeyState::Up;
			}
		}
	}
	
	KeyState InputSystem::GetStateForKey(KeyCode key, const std::unordered_map<KeyCode, KeyState>& stateMap) const
	{
		auto it = stateMap.find(key);
		return (it != stateMap.end()) ? it->second : KeyState::Up;
	}
	
	void InputSystem::UpdateKeyState(KeyCode key, bool isDown, std::unordered_map<KeyCode, KeyState>& stateMap)
	{
		if (key == KeyCode::None) 
		{
			return;
		}

		auto it = stateMap.find(key);
		KeyState currentState = (it != stateMap.end()) ? it->second : KeyState::Up;

		// Key pressed down
		if (isDown)
		{
			if (currentState == KeyState::Up || currentState == KeyState::Released)
			{
				stateMap[key] = KeyState::Down;
			}
			// If already Down or Held, keep as is (will become Held next frame)
		}
		else  // Key released
		{ 			
			if (currentState == KeyState::Down || currentState == KeyState::Held)
			{
				stateMap[key] = KeyState::Released;
			}
			// If already Up or Released, keep as is (will become Up next frame)
		}
	}
	
	void InputSystem::SetKeyState(KeyCode key, bool isDown)
	{
		UpdateKeyState(key, isDown, m_keyStates);
	}
	
	void InputSystem::SetMouseButtonState(KeyCode button, bool isDown)
	{
		UpdateKeyState(button, isDown, m_mouseButtonStates);
	}
	
	void InputSystem::SetMousePosition(i32 x, i32 y)
	{
		if (!m_rawMouseEnabled)
		{
			m_mouseDeltaX = x - m_mouseX;
			m_mouseDeltaY = y - m_mouseY;
		}
		
		m_mouseX = x;
		m_mouseY = y;
	}
	
	void InputSystem::SetMouseDelta(i32 deltaX, i32 deltaY)
	{
		m_mouseDeltaX = deltaX;
		m_mouseDeltaY = deltaY;
	}
	
	void InputSystem::SetModifierKeys(ModifierKeys modifiers)
	{
		m_currentModifiers = modifiers;
	}
}

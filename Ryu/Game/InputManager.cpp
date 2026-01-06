#include "Game/InputManager.h"

namespace Ryu::Game
{
	InputManager::InputManager(const Window::InputSystem& windowInput, Event::EventDispatcher& dispatcher)
        : m_windowInput(&windowInput)
	{
        // Subscribe to events for action system
        m_keyListener.Initialize(dispatcher, [this](const Window::KeyEvent& e) 
        {
            if (e.State == Window::KeyState::Down)
            {
                for (auto& [name, action] : m_actions)
                {
                    if (action.first == e.KeyCode  // Keycode matches
                        && action.second)          // Valid action callback
                    {
                        action.second();
                    }
                }
            }
        });
	}    

    bool InputManager::IsKeyDown(Window::KeyCode key) const
    {
        return m_windowInput ? m_windowInput->IsKeyDown(key) : false;
    }

    bool InputManager::IsKeyPressed(Window::KeyCode key) const
    {
        return m_windowInput ? m_windowInput->IsKeyPressed(key) : false;
    }
    
    bool InputManager::IsKeyReleased(Window::KeyCode key) const
    {
        return m_windowInput ? m_windowInput->IsKeyReleased(key) : false;
    }
    
    bool InputManager::IsMouseButtonDown(Window::KeyCode button) const
    {
        return m_windowInput ? m_windowInput->IsMouseButtonDown(button) : false;
    }
    
    bool InputManager::IsMouseButtonPressed(Window::KeyCode button) const
    {
        return m_windowInput ? m_windowInput->IsMouseButtonPressed(button) : false;
    }
    
    void InputManager::GetMousePosition(i32& x, i32& y) const
    {
        if (m_windowInput)
        {
            m_windowInput->GetMousePosition(x, y);
        }
    }
    
    void InputManager::GetMouseDelta(i32& dx, i32& dy) const
    {
		if (m_windowInput)
		{
			m_windowInput->GetMouseDelta(dx, dy);
		}
    }
    
    bool InputManager::HasModifier(Window::ModifierKeys modifier) const
    {
        return m_windowInput ? m_windowInput->HasModifier(modifier) : false;
    }
    
    void InputManager::BindAction(const std::string& actionName, Window::KeyCode key, ActionCallback callback)
    {
		m_actions[actionName] = { key, std::move(callback) };
    }
    
    void InputManager::TriggerAction(const std::string& actionName)
    {
        auto it = m_actions.find(actionName);
        if (it != m_actions.end() && it->second.second) 
        {
            it->second.second();  // Fire action callback
        }
    }
}

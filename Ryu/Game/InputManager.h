#pragma once
#include "Application/Window/Input/InputSystem.h"
#include "Application/Event/ScopedListener.h"

namespace Ryu::Game
{
	class InputManager
	{
	public:
		using ActionCallback = std::function<void()>;

	public:
		InputManager(const Window::InputSystem& windowInput, Event::EventDispatcher& dispatcher);
		void Update() { /* Reserved for frame based processing */ }

		bool IsKeyDown(Window::KeyCode key) const;
		bool IsKeyPressed(Window::KeyCode key) const;
		bool IsKeyReleased(Window::KeyCode key) const;
		bool IsMouseButtonDown(Window::KeyCode button) const;
		bool IsMouseButtonPressed(Window::KeyCode button) const;
		void GetMousePosition(i32& x, i32& y) const;
		void GetMouseDelta(i32& dx, i32& dy) const;
		bool HasModifier(Window::ModifierKeys modifier) const;

		void BindAction(const std::string& actionName, Window::KeyCode key, ActionCallback callback);
		void TriggerAction(const std::string& actionName);

	private:
		const Window::InputSystem* m_windowInput{ nullptr };
		std::unordered_map<std::string, std::pair<Window::KeyCode, ActionCallback>> m_actions;
		
		Event::ScopedListener<Window::KeyEvent>         m_keyListener;
		Event::ScopedListener<Window::MouseButtonEvent> m_mouseListener;
	};
}

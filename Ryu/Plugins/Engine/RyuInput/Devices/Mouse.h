#pragma once
#include <RyuInput/KeyCode.h>
#include <unordered_map>

namespace Ryu::Input
{
	struct InputCallbacks;

	class RYU_API Mouse
	{
		friend class InputSystem;

	private:
		void Create(std::vector<InputCallbacks>* callbacks);
		// Handles up and down events
		void OnClick(MouseButton button, LPARAM lParam, bool isDown);
		// Handles double click events
		void OnDblClick(MouseButton button, WPARAM wParam, LPARAM lParam);
		// Handles mouse move events
		void OnMove(WPARAM wParam, LPARAM lParam);
		void OnWheel(WPARAM wParam, LPARAM lParam);

	private:
		std::vector<InputCallbacks>* m_callbacks;
		i32 m_posX;
		i32 m_posY;
	};
}

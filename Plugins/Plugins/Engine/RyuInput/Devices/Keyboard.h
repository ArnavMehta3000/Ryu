#include <Plugins/Engine/RyuInput/KeyCode.h>
#include <unordered_map>

namespace Ryu::Input
{
	struct InputCallbacks;

	class RYU_API Keyboard
	{
		friend class InputSystem;
	public:
		struct KeyState
		{
			bool IsDown = false;
			bool WasDown = false;
		};

		using KeyMap = std::unordered_map<KeyCode, KeyState>;

	private:
		void Create(InputCallbacks* callbacks);
		void Read(WPARAM wParam, LPARAM lParam);
		void UpdateKeyState(KeyCode keyCode, bool isKeyReleased, bool wasKeyDown);
		void OnKeyUp(KeyCode key);
		void OnKeyDown(KeyCode key);

	private:
		InputCallbacks*   m_callbacks;
	};
}

#include "Keyboard.h"
#include <Plugins/Engine/RyuInput/Internal/Log.h>
#include <Plugins/Engine/RyuInput/KeyCode.h>
#include <Plugins/Engine/RyuInput/InputEvents.h>

namespace Ryu::Input
{
	namespace
	{
#define KEY_MAP(keycode) { KeyCode::keycode, Keyboard::KeyState{} }
		// Constant mapping of keycodes to virtual keys.
		static Keyboard::KeyMap s_keyStates =
		{
		};
#undef KEY_MAP
	}

	void Keyboard::Create(InputCallbacks* inputCallbacks)
	{
		RYU_PLUGIN_ASSERT(inputCallbacks, "InputCallbacks is nullptr");

		m_callbacks = inputCallbacks;
	}

	void Keyboard::Read(WPARAM wParam, LPARAM lParam)
	{
		WORD vkCode              = LOWORD(wParam);
		const WORD keyFlags      = HIWORD(lParam);
		WORD scanCode            = LOBYTE(keyFlags);
		const BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED;
		if (isExtendedKey)
		{
			scanCode = MAKEWORD(scanCode, 0xE0);
		}

		const BOOL wasKeyDown    = (keyFlags & KF_REPEAT) == KF_REPEAT;
		const BOOL isKeyReleased = (keyFlags & KF_UP) == KF_UP;  // transition-state flag, 1 on key up

		auto isValid = IsValidKeyCodeValue(vkCode);
		if (!isValid)
		{
			// Code is invalid
			return;
		}

		const KeyCode keyCode = isValid.value();

		// Updates key state (including generic shift/ctrl/alt)
		UpdateKeyState(keyCode, isKeyReleased, wasKeyDown);

		// Check for modifier key
		switch (vkCode)
		{
		case VK_SHIFT:   // converts to VK_LSHIFT or VK_RSHIFT
		case VK_CONTROL: // converts to VK_LCONTROL or VK_RCONTROL
		case VK_MENU:    // converts to VK_LMENU or VK_RMENU
		{
			vkCode = LOWORD(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));

			// Update directional keys
			isValid = IsValidKeyCodeValue(vkCode);
			if (isValid)
			{
				UpdateKeyState(isValid.value(), isKeyReleased, wasKeyDown);
			}
			break;
		}
		}				
	}

	void Keyboard::UpdateKeyState(KeyCode keyCode, bool isKeyReleased, bool wasKeyDown)
	{
		KeyState& keyState = s_keyStates[keyCode];
		keyState.IsDown = !isKeyReleased;
		keyState.WasDown = wasKeyDown;

		if (keyState.WasDown && !keyState.IsDown)
		{
			OnKeyUp(keyCode);
		}
		else if (!keyState.WasDown && keyState.IsDown)
		{
			OnKeyDown(keyCode);
		}
	}

	void Keyboard::OnKeyUp(KeyCode key)
	{
		if (m_callbacks->OnKeyDown)
		{
			m_callbacks->OnKeyDown(Events::OnKeyDown(key));
		}
	}

	void Keyboard::OnKeyDown(KeyCode key)
	{
		if (m_callbacks->OnKeyUp)
		{
			m_callbacks->OnKeyUp(Events::OnKeyUp(key));
		}
	}
}
#include "InputSystem.h"
#include <Plugins/Engine/InputSystem/Log.h>

namespace Ryu
{
	WNDPROC InputSystem::s_originalWndProc{ nullptr };

	InputSystem::InputSystem() : IPlugin(PluginLoadOrder::PostInit)
		, m_hWnd(nullptr)
	{
	}

	InputSystem::~InputSystem()
	{
		RYU_PLUGIN_TRACE("Destructed InputSystem plugin");
	}

	bool InputSystem::Initialize(const PluginAPI& api)
	{
		RYU_PLUGIN_INFO("Initializing InputSystem plugin");

		m_hWnd = api.Window;
		RYU_PLUGIN_ASSERT(m_hWnd != nullptr, "Invalid window handle");

		s_originalWndProc = (WNDPROC)::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)&InputSystem::InputWndProc);
		return s_originalWndProc != nullptr;
	}

	void InputSystem::Shutdown()
	{
		RYU_PLUGIN_DEBUG("Shutting down InputSystem plugin");
		// Restore original WndProc
		::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)s_originalWndProc);
	}

	LRESULT InputSystem::InputWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			case WM_KEYDOWN:
			{
				// Check if esc is pressed
				if (wParam == VK_ESCAPE)
				{
					::PostQuitMessage(0);
				}
			}
		}

		return ::CallWindowProc(InputSystem::s_originalWndProc, hWnd, msg, wParam, lParam);
	}
}

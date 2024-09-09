#include "InputSystem.h"
#include <Plugins/Engine/RyuInput/Internal/Log.h>

namespace Ryu::Input
{
	namespace
	{
		WNDPROC s_originalWndProc{ nullptr };
		InputSystem* s_instance = nullptr;
	}
	
	InputSystem::InputSystem() : IPlugin(PluginLoadOrder::PostInit)
		, m_hWnd(nullptr)
	{
		RYU_PLUGIN_TRACE("Constructed InputSystem plugin");
		s_instance = this;
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

	void InputSystem::SetInputCallbacks(const InputCallbacks& callbacks)
	{
		m_callbacks = callbacks;
		RYU_PLUGIN_DEBUG("Input callbacks set");
	}

	const InputCallbacks& InputSystem::GetInputCallbacks() const
	{
		return m_callbacks;
	}

	LRESULT InputSystem::InputWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		const InputCallbacks& callbacks = s_instance->GetInputCallbacks();
		switch (msg)
		{
			case WM_KEYDOWN:
			{
				// Check if esc is pressed
				if (wParam == VK_ESCAPE)
				{
					::PostQuitMessage(0);
				}

				if (callbacks.OnKeyDown)
				{
					callbacks.OnKeyDown(Events::OnKeyDown{});
				}

				break;
			}
		}

		return ::CallWindowProc(s_originalWndProc, hWnd, msg, wParam, lParam);
	}
}

#include "InputSystem.h"
#include <Plugins/Engine/RyuInput/Internal/Log.h>


namespace Ryu::Input
{
	namespace
	{
		WNDPROC s_originalWndProc{ nullptr };
		InputSystem* s_instance{ nullptr };
	}
	
	InputSystem::InputSystem() : IPlugin()
		, m_keyboard()
		, m_mouse()
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

		m_hWnd = api.App->GetWindow();
		RYU_PLUGIN_ASSERT(m_hWnd != nullptr, "Invalid window handle");

		m_keyboard.Create(&m_callbacks);
		m_mouse.Create(&m_callbacks);

		s_originalWndProc = (WNDPROC)::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)&InputSystem::InputWndProc);
		return s_originalWndProc != nullptr;
	}

	void InputSystem::Shutdown()
	{
		RYU_PLUGIN_DEBUG("Shutting down InputSystem plugin");
		// Restore original WndProc
		::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)s_originalWndProc);
	}

	void InputSystem::AddInputCallbacks(const InputCallbacks& callbacks)
	{
		m_callbacks.push_back(callbacks);
		RYU_PLUGIN_DEBUG("Input callbacks set");
	}

	LRESULT InputSystem::InputWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (!s_instance)
		{
			return ::CallWindowProc(s_originalWndProc, hWnd, msg, wParam, lParam);
		}

		switch (msg)
		{
		// Keyboard messages
		case WM_KEYDOWN:    FALLTHROUGH;
		case WM_SYSKEYDOWN: FALLTHROUGH;
		case WM_KEYUP:      FALLTHROUGH;
		case WM_SYSKEYUP:
		{
			s_instance->m_keyboard.Read(wParam, lParam);
			break;
		}

		// Mouse messages
		case WM_MOUSEACTIVATE:
			return MA_ACTIVATEANDEAT;

		case WM_LBUTTONDOWN:
		{
			s_instance->m_mouse.OnClick(MouseButton::LeftButton, lParam, true);
			break;
		}

		case WM_RBUTTONDOWN:
		{
			s_instance->m_mouse.OnClick(MouseButton::RightButton, lParam, true);
			break;
		}
		case WM_MBUTTONDOWN:
		{
			s_instance->m_mouse.OnClick(MouseButton::MiddleButton, lParam, true);
			break;
		}
		case WM_XBUTTONDOWN:
		{
			s_instance->m_mouse.OnClick(HIWORD(wParam) == XBUTTON1 ? 
				MouseButton::XButton1 : MouseButton::XButton2, lParam, true);
			break;
		}
		case WM_LBUTTONUP:
		{
			s_instance->m_mouse.OnClick(MouseButton::LeftButton, lParam, false);
			break;
		}
		case WM_RBUTTONUP:
		{
			s_instance->m_mouse.OnClick(MouseButton::RightButton, lParam, false);
			break;
		}
		case WM_MBUTTONUP:
		{
			s_instance->m_mouse.OnClick(MouseButton::MiddleButton, lParam, false);
			break;
		}
		case WM_XBUTTONUP:
		{
			s_instance->m_mouse.OnClick(HIWORD(wParam) == XBUTTON1 ? 
				MouseButton::XButton1 : MouseButton::XButton2, lParam, false);
			break;
		}
		case WM_LBUTTONDBLCLK:
		{
			s_instance->m_mouse.OnDblClick(MouseButton::LeftButton, wParam, lParam);
			break;
		}
		case WM_RBUTTONDBLCLK:
		{
			s_instance->m_mouse.OnDblClick(MouseButton::RightButton, wParam, lParam);
			break;
		}
		case WM_MBUTTONDBLCLK:
		{
			s_instance->m_mouse.OnDblClick(MouseButton::MiddleButton, wParam, lParam);
			break;
		}
		case WM_XBUTTONDBLCLK:
		{
			s_instance->m_mouse.OnDblClick(HIWORD(wParam) == XBUTTON1 ?
				MouseButton::XButton1 : MouseButton::XButton2, wParam, lParam);
			break;
		}
		case WM_MOUSEMOVE:
		{
			s_instance->m_mouse.OnMove(wParam, lParam);
			break;
		}

		case WM_MOUSEWHEEL:
		{
			s_instance->m_mouse.OnWheel(wParam, lParam);
			break;
		}
		}

		return ::CallWindowProc(s_originalWndProc, hWnd, msg, wParam, lParam);
	}
}

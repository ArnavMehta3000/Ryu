#include "InputSystem.h"
#include <Plugins/Engine/RyuInput/Generated/PluginData.h>
#include <Plugins/Engine/RyuInput/Generated/RyuInputLog.h>

RYU_REGISTER_PLUGIN(Ryu::Input::InputSystem)

namespace Ryu::Input
{
	InputSystem::InputSystem() : PluginBase()
		, m_originalWndProc(nullptr)
		, m_keyboard()
		, m_mouse()
		, m_hWnd(nullptr)
	{
		PLUGIN_TRACE("Constructed RyuInput plugin");

		if (!GetInstance())
		{
			SetPluginInstance(this);
		}
	}

	InputSystem::~InputSystem()
	{
		PLUGIN_TRACE("Destructed RyuInput plugin");
	}

	bool InputSystem::Initialize(const PluginAPI& api)
	{
		PLUGIN_INFO("Initializing RyuInput plugin");

		m_hWnd = api.App->GetWindow();
		PLUGIN_ASSERT(m_hWnd != nullptr, "Invalid window handle");

		m_keyboard.Create(&m_callbacks);
		m_mouse.Create(&m_callbacks);

		m_originalWndProc = (WNDPROC)::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)&InputSystem::InputWndProc);
		return m_originalWndProc != nullptr;
	}

	void InputSystem::Shutdown()
	{
		PLUGIN_DEBUG("Shutting down RyuInput plugin");
		// Restore original WndProc
		::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_originalWndProc);
	}

	void InputSystem::AddInputCallbacks(const InputCallbacks& callbacks)
	{
		m_callbacks.push_back(callbacks);
		PLUGIN_DEBUG("Input callbacks set");
	}

	LRESULT InputSystem::InputWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		InputSystem* instance = GetInstance();
		if (!instance)
		{
			return ::DefWindowProc(hWnd, msg, wParam, lParam);
		}


		switch (msg)
		{
		// Keyboard messages
		case WM_KEYDOWN:    FALLTHROUGH;
		case WM_SYSKEYDOWN: FALLTHROUGH;
		case WM_KEYUP:      FALLTHROUGH;
		case WM_SYSKEYUP:
		{
			instance->m_keyboard.Read(wParam, lParam);
			break;
		}

		// Mouse messages
		case WM_MOUSEACTIVATE:
			return MA_ACTIVATEANDEAT;

		case WM_LBUTTONDOWN:
		{
			instance->m_mouse.OnClick(MouseButton::LeftButton, lParam, true);
			break;
		}

		case WM_RBUTTONDOWN:
		{
			instance->m_mouse.OnClick(MouseButton::RightButton, lParam, true);
			break;
		}
		case WM_MBUTTONDOWN:
		{
			instance->m_mouse.OnClick(MouseButton::MiddleButton, lParam, true);
			break;
		}
		case WM_XBUTTONDOWN:
		{
			instance->m_mouse.OnClick(HIWORD(wParam) == XBUTTON1 ? 
				MouseButton::XButton1 : MouseButton::XButton2, lParam, true);
			break;
		}
		case WM_LBUTTONUP:
		{
			instance->m_mouse.OnClick(MouseButton::LeftButton, lParam, false);
			break;
		}
		case WM_RBUTTONUP:
		{
			instance->m_mouse.OnClick(MouseButton::RightButton, lParam, false);
			break;
		}
		case WM_MBUTTONUP:
		{
			instance->m_mouse.OnClick(MouseButton::MiddleButton, lParam, false);
			break;
		}
		case WM_XBUTTONUP:
		{
			instance->m_mouse.OnClick(HIWORD(wParam) == XBUTTON1 ? 
				MouseButton::XButton1 : MouseButton::XButton2, lParam, false);
			break;
		}
		case WM_LBUTTONDBLCLK:
		{
			instance->m_mouse.OnDblClick(MouseButton::LeftButton, wParam, lParam);
			break;
		}
		case WM_RBUTTONDBLCLK:
		{
			instance->m_mouse.OnDblClick(MouseButton::RightButton, wParam, lParam);
			break;
		}
		case WM_MBUTTONDBLCLK:
		{
			instance->m_mouse.OnDblClick(MouseButton::MiddleButton, wParam, lParam);
			break;
		}
		case WM_XBUTTONDBLCLK:
		{
			instance->m_mouse.OnDblClick(HIWORD(wParam) == XBUTTON1 ?
				MouseButton::XButton1 : MouseButton::XButton2, wParam, lParam);
			break;
		}
		case WM_MOUSEMOVE:
		{
			instance->m_mouse.OnMove(wParam, lParam);
			break;
		}

		case WM_MOUSEWHEEL:
		{
			instance->m_mouse.OnWheel(wParam, lParam);
			break;
		}
		}

		return ::CallWindowProc(instance->m_originalWndProc, hWnd, msg, wParam, lParam);
	}
}

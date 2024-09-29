#include "Window.h"
#include <utility>
#include <Core/Log/Log.h>
#include <Core/Log/Logger.h>

namespace Ryu
{
	Window::Window(Window::Config wndConfig)
		: SubObject(L"RyuWndClass", wndConfig.Title, wndConfig.Instance, wndConfig.Icon)
		, m_config(wndConfig)
		, m_hWnd(nullptr)
		, m_isOpen(false)
		, m_activated(false)
	{
	}

	Window::~Window()
	{
		m_hWnd = nullptr;
	}

	NODISCARD bool Window::Init()
	{
		if (!SubObject::RegisterNewClass())
		{
			RYU_CORE_LOG_ERROR(Core, "Failed to register new window class.");
			return false;
		}

		// Get monitor dimensions
		i32 screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
		i32 screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

		i32 posX = std::max((screenWidth - m_config.Width) / 2, 0);
		i32 posY = std::max((screenHeight - m_config.Height) / 2, 0);

		// ----- Get dimensions -----
		DWORD flags = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		flags |= static_cast<DWORD>(m_config.Type);

		RECT rect = { posX, posY, posX + m_config.Width, posY + m_config.Height };
		::AdjustWindowRectEx(&rect, flags, false, WS_EX_APPWINDOW);

		posX = rect.left;
		posY = rect.top;
		m_config.Width = rect.right - rect.left;
		m_config.Height = rect.bottom - rect.top;

		// ----- Create window -----
		m_hWnd = ::CreateWindowEx(
			WS_EX_APPWINDOW,
			m_className.c_str(),
			m_title.c_str(),
			flags,
			posX,
			posY,
			m_config.Width,
			m_config.Height,
			nullptr,
			nullptr,
			m_hInstance,
			this);
		
		if (m_hWnd)
		{
			m_isOpen = true;
			return true;
		}

		const std::wstring msg = L"Window creation failed with error: " + std::to_wstring(GetLastError());
		::MessageBox(nullptr, msg.c_str(), L"Error", MB_ICONEXCLAMATION | MB_OK);
		
		return false;
	}

	void Window::Show()
	{
		::ShowWindow(m_hWnd, SW_SHOW);
		::UpdateWindow(m_hWnd);
		::SetForegroundWindow(m_hWnd);
		::SetFocus(m_hWnd);
		m_activated = true;
	}

	void Window::ToggleMaximize()
	{
		WINDOWPLACEMENT wPos{};
		::GetWindowPlacement(m_hWnd, &wPos);
		::ShowWindow(m_hWnd, (wPos.showCmd == SW_MAXIMIZE) ? SW_NORMAL : SW_MAXIMIZE);
		RYU_CORE_LOG_TRACE(Core, "Toggled window to {}", (wPos.showCmd == SW_MAXIMIZE) ? "normal" : "maximized");
	}

	void Window::PumpMessages()
	{
		MSG msg{};

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				m_isOpen = false;
			}
		}
	}

	bool Window::IsMaximized()
	{
		WINDOWPLACEMENT placement{};
		::GetWindowPlacement(m_hWnd, &placement);
		return placement.showCmd == SW_MAXIMIZE;
	}

	bool Window::HasStyle(DWORD style)
	{
		DWORD dwStyle = (DWORD)::GetWindowLong(m_hWnd, GWL_STYLE);
		return (dwStyle & style) != 0;
	}

	void Window::SetStyle(DWORD flagsToDisable, DWORD flagsToEnable)
	{
		DWORD style = ::GetWindowLong(m_hWnd, GWL_STYLE);
		::SetWindowLong(m_hWnd, GWL_STYLE, (style & ~flagsToDisable) | flagsToEnable);
	}

	void Window::SetStyleEx(DWORD flagsToDisable, DWORD flagsToEnable)
	{
		DWORD styleEx = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, (styleEx & ~flagsToDisable) | flagsToEnable);
	}

	void Window::ModifyClassStyle(DWORD flagsToDisable, DWORD flagsToEnable)
	{
		DWORD style = ::GetWindowLong(m_hWnd, GCL_STYLE);
		::SetClassLong(m_hWnd, GCL_STYLE, (style & ~flagsToDisable) | flagsToEnable);
	}

	LRESULT Window::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_ERASEBKGND:
			return 0;

		case WM_CLOSE: FALLTHROUGH;
		case WM_DESTROY:
		{
			m_isOpen = false;
			return 0;
		}

		case WM_GETMINMAXINFO:
		{
			OnGetMinMaxInfo((MINMAXINFO*)(lParam));
			return 0;
		}

		case WM_EXITSIZEMOVE:
		{
			OnExitSizeMove();
			break;
		}

		case WM_SYSCOMMAND:
		{
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		}
		}

		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
		
	void Window::OnGetMinMaxInfo(MINMAXINFO* minmax)
	{
		minmax->ptMinTrackSize.x = 400;
		minmax->ptMinTrackSize.y = 300;
	}
	
	void Window::OnExitSizeMove()
	{
		RECT r{};
		::GetClientRect(m_hWnd, &r);
		m_config.Width = r.right - r.left;
		m_config.Height = r.bottom - r.top;

		RYU_CORE_LOG_TRACE(Core, "Window client resized to: {}x{}", m_config.Width, m_config.Height);

		if (m_resizeCallback)
		{
			m_resizeCallback(m_config.Width, m_config.Height);
		}
	}
	
}

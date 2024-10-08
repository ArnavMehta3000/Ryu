#include "Window.h"
#include "Utils/MessageBox.h"
#include <dwmapi.h>

namespace Ryu::App
{
	Window::Window(const std::wstring& name, u32 width, u32 height, IWindowEventListener* eventListener)
		: WindowBase(GetModuleHandle(NULL), L"RyuWindow")
		, m_eventListener(eventListener)
		, m_width(width)
		, m_height(height)
		, m_name(name)
	{
	}

	void Window::Create()
	{
		const DWORD flags = WS_OVERLAPPEDWINDOW;
		const DWORD exFlags = WS_EX_APPWINDOW;

		const u32 screenWidth = u32(::GetSystemMetrics(SM_CXSCREEN));
		const u32 screenHeight = u32(::GetSystemMetrics(SM_CYSCREEN));

		i32 posX = std::max((screenWidth - m_width) / 2u, 0u);
		i32 posY = std::max((screenHeight - m_height) / 2u, 0u);

		RECT rect = { posX, posY, LONG(posX + m_width), LONG(posY + m_height) };
		::AdjustWindowRectEx(&rect, flags, FALSE, exFlags);

		posX     = rect.left;
		posY     = rect.top;
		m_width  = rect.right - rect.left;
		m_height = rect.bottom - rect.top;

		m_hWnd = CreateWindowEx(
			exFlags,
			GetBaseName().c_str(),
			m_name.c_str(),
			flags,
			posX, posY,
			m_width, m_height,
			nullptr, nullptr,
			GetHInstance(),
			this);

		if (!m_hWnd)
		{
			Utils::MessageBoxDesc desc
			{
				.Window = nullptr,
				.Title = "Window Creation Error",
				.Text = "Failed to create window GetLastError() = " + std::to_string(GetLastError()),
				.Flags = { Utils::MessagBoxButton::Ok, Utils::MessageBoxIcon::Error }
			};

			Utils::ShowMessageBox(desc);
			return;
		}

		SetDarkTheme();

		::ShowWindow(m_hWnd, SW_SHOW);
		::UpdateWindow(m_hWnd);
		::SetForegroundWindow(m_hWnd);
		::SetFocus(m_hWnd);
	}

	void Window::PumpMessages()
	{
		MSG msg{};

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void Window::SetDarkTheme()
	{
		// Dark Mode Ref: https://learn.microsoft.com/en-us/windows/apps/desktop/modernize/ui/apply-windows-themes

		constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
		BOOL value = TRUE;
		::DwmSetWindowAttribute(m_hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
	}

	LRESULT Window::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_ERASEBKGND:
			return 0;

		case WM_CLOSE:
		{
			Events::OnWindowClose e;
			e.Window = this;

			SendEvent(e);
			break;
		}

		case WM_DESTROY:
			PostQuitMessage(EXIT_SUCCESS);
			return 0;

		case WM_GETMINMAXINFO:
		{
			MINMAXINFO* minmax = ((MINMAXINFO*)(lParam));
			minmax->ptMinTrackSize.x = 400;
			minmax->ptMinTrackSize.y = 300;
			return 0;
		}

		case WM_SYSCOMMAND:
		{
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		}

		case WM_SIZE:
		{
			// Contains client dimensions
			m_width = LOWORD(lParam);
			m_height = HIWORD(lParam);

			Events::OnWindowStateChange e;
			e.Window = this;
			e.State = (wParam == SIZE_MAXIMIZED) ? WindowState::Maximized : 
					  (wParam == SIZE_MINIMIZED) ? WindowState::Minimized : WindowState::Restored;
			e.Width = m_width;
			e.Height = m_height;

			SendEvent(e);
			break;
		}

		case WM_EXITSIZEMOVE:
		{
			// Send window resize event after the resize id done
			// We already know the new size (cached during WM_SIZE)

			Events::OnWindowResize e;
			e.Window = this;
			e.Width = m_width;
			e.Height = m_height;

			SendEvent(e);
			break;
		}

		case WM_SETFOCUS:
		{
			Events::OnWindowFocusChange e;
			e.Window = this;
			e.Focus = WindowFocus::Gained;
			
			SendEvent(e);
			break;
		}

		case WM_KILLFOCUS:
		{
			Events::OnWindowFocusChange e;
			e.Window = this;
			e.Focus = WindowFocus::Lost;

			SendEvent(e);
			break;
		}
		}

		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

}

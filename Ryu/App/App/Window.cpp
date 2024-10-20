#include "Window.h"
#include "Utils/MessageBox.h"
#include <dwmapi.h>

namespace Ryu::App
{
	Window::Window(const std::wstring& name, u32 width, u32 height, IWindowEventListener* eventListener)
		: WindowBase(GetModuleHandle(NULL), L"RyuWindow")
		, m_eventListener(eventListener)
		, m_name(name)
		, m_minimized(false)
		, m_maximized(false)
		, m_resizing(false)
	{

		m_width = width;
		m_height = height;
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

		case WM_ENTERSIZEMOVE:
		{
			m_resizing = true;
			break;
		}

		case WM_SIZE:
		{
			// Contains client dimensions
			m_width = LOWORD(lParam);
			m_height = HIWORD(lParam);

			Events::OnWindowStateChange e;
			e.Window = this;
			e.Width = m_width;
			e.Height = m_height;

			bool shouldResize = false;

			if (wParam == SIZE_MINIMIZED)
			{
				m_minimized = true;
				m_maximized = false;
				e.State = WindowState::Minimized;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_minimized = false;
				m_maximized = true;
				shouldResize = true;
				e.State = WindowState::Maximized;
			}
			else if (wParam == SIZE_RESTORED)
			{
				// Restoring from minimized state?
				if (m_minimized)
				{
					m_minimized = false;
					shouldResize = true;
				}
				// Restoring from maximized state?
				else if (m_maximized)
				{
					m_maximized = false;
					shouldResize = true;
				}
				else if (m_resizing)
				{
					// If user is dragging the resize bars, we do not resize
					// the buffers here because as the user continuously
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is
					// done resizing the window and releases the resize bars, which
					// sends a WM_EXITSIZEMOVE message.
				}
				// API call such as SetWindowPos or IDXGISwapChain::SetFullscreenState
				else
				{
					shouldResize = true;
				}

				e.State = WindowState::Restored;
			}

			SendEvent(e);

			// Send resize event if needed
			if (shouldResize)
			{
				Events::OnWindowResize resize;
				resize.Window = this;
				resize.Width  = e.Width;
				resize.Height = e.Height;

				SendEvent(resize);
			}
			break;
		}

		case WM_EXITSIZEMOVE:
		{
			// Send window resize event after the resize id done
			// We already know the new size (cached during WM_SIZE)
			m_resizing = false;

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

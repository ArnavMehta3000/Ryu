#include "Window.h"
#include <utility>

namespace Ryu
{
	Window::Window(Window::Config wndConfig)
		: m_config(wndConfig)
		, m_isOpen(false)
		, m_hasFocus(false)
	{
	}

	Window::~Window()
	{
		m_hWnd = nullptr;
	}

	NODISCARD bool Window::Create()
	{
		if (!m_config.Windowed && m_config.Borderless)
		{
			return false;
		}

		// ----- Create window class -----
		WNDCLASSEX wc{};
		ZeroMemory(&wc, sizeof(WNDCLASSEX));

		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc   = &Ryu::Window::WindowProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = m_config.Instance;
		wc.hIcon         = LoadIcon(nullptr, IDI_WINLOGO);
		wc.hIconSm       = wc.hIcon;
		wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = nullptr;
		wc.lpszMenuName  = nullptr;
		wc.lpszClassName = m_config.Title;

		::RegisterClassEx(&wc);

		// Get monitor dimensions
		i32 screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
		i32 screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

		i32 posX = 0, posY = 0;

		if (m_config.Windowed)
		{
			posX = std::max((screenWidth - m_config.Width) / 2, 0);
			posY = std::max((screenHeight - m_config.Height) / 2, 0);
		}
		else
		{
			m_config.Width = screenWidth;
			m_config.Height = screenHeight;

			// If full screen set the screen to maximum size of the users desktop and 32bit.
			DEVMODE dmScreenSettings;
			ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));

			dmScreenSettings.dmSize       = sizeof(DEVMODE);
			dmScreenSettings.dmPelsWidth  = m_config.Width;
			dmScreenSettings.dmPelsHeight = m_config.Height;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			if (!m_config.Borderless)
				::ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
			// Position will be (0,0) if borderless
		}

		// ----- Create window -----
		u32 flags = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		flags |= m_config.Borderless ? WS_POPUP : WS_OVERLAPPEDWINDOW;

		RECT rect = { posX, posY, posX + m_config.Width, posY + m_config.Height };
		::AdjustWindowRectEx(&rect, flags, false, WS_EX_APPWINDOW);

		posX = rect.left;
		posY = rect.top;
		m_config.Width = rect.right - rect.left;
		m_config.Height = rect.bottom - rect.top;

		m_hWnd = ::CreateWindowEx(
			WS_EX_APPWINDOW,
			m_config.Title,
			m_config.Title,
			flags,
			posX,
			posY,
			m_config.Width,
			m_config.Height,
			nullptr,
			nullptr,
			m_config.Instance,
			this);

		if (m_hWnd)
		{
			m_isOpen = true;
			return true;
		}

		return false;
	}

	void Window::Show()
	{
		ShowWindow(m_hWnd, SW_SHOW);
		SetForegroundWindow(m_hWnd);
		SetFocus(m_hWnd);
		m_hasFocus = true;
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
				m_hasFocus = false;
			}
		}
	}

	LRESULT Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		Window* pThis = NULL;

		if (uMsg == WM_NCCREATE)
		{
			CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
			pThis = (Window*)pCreate->lpCreateParams;
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

			pThis->m_hWnd = hWnd;
		}
		else
		{
			pThis = (Window*)(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		}

		return pThis ? pThis->HandleMessage(uMsg, wParam, lParam) : ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	LRESULT Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_CLOSE: FALLTHROUGH;
		case WM_DESTROY:
		{
			m_isOpen = false;
			m_hasFocus = false;
			return 0;
		}

		case WM_GETMINMAXINFO:
		{
			MINMAXINFO* info = (MINMAXINFO*)(lParam);
			info->ptMinTrackSize.x = 380;
			info->ptMinTrackSize.y = 200;
			return 0;
		}

		case WM_ERASEBKGND:
			return 0;

		case WM_SIZE:
		{
			RECT r{};
			::GetClientRect(m_hWnd, &r);
			m_config.Width = r.right - r.left;
			m_config.Height = r.bottom - r.top;
			break;
		}

		case WM_EXITSIZEMOVE:
		{
			if (wParam == SC_SIZE)
			{
				if (m_resizeCallback)
				{
					m_resizeCallback(m_config.Width, m_config.Height);
				}
			}

			break;
		}
		}

		return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	}
}
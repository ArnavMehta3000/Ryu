#include "Window/Window.h"
#include "Utils/StringConv.h"
#include <stdexcept>

namespace Ryu::Window
{
	Window::Window(const WindowConfig& config)
		: m_config(config)
		, m_prevSize(config.Width, config.Height)
		, m_currentSize(config.Width, config.Height)
	{
		RegisterWindowClass();
	}

	Window::~Window()
	{
		Destroy();
	}

	bool Window::Create()
	{
		if (m_hwnd)
		{
			return true;
		}

		DWORD style = GetWindowStyle();
		DWORD exStyle = GetExtendedWindowStyle();

		RECT windowRect{ 0, 0, static_cast<LONG>(m_config.Width), static_cast<LONG>(m_config.Height) };
		::AdjustWindowRectEx(&windowRect, style, FALSE, exStyle);

		const i32 windowWidth = windowRect.right - windowRect.left;
		const i32 windowHeight = windowRect.bottom - windowRect.top;

		const std::wstring windowTitle = Utils::ToWideStr(m_config.Title);

		// Center on screen
		if (m_config.X == CW_USEDEFAULT && m_config.Y == CW_USEDEFAULT)
		{
			const HDC screenDC = ::GetDC(nullptr);
			m_config.X         = ::GetDeviceCaps(screenDC, HORZRES) / 2 - windowWidth / 2;
			m_config.Y         = ::GetDeviceCaps(screenDC, VERTRES) / 2 - windowHeight / 2;
			::ReleaseDC(nullptr, screenDC);
		}

		m_hwnd = ::CreateWindowExW(
			exStyle,
			s_className,
			windowTitle.c_str(),
			style,
			m_config.X, m_config.Y,
			windowWidth, windowHeight,
			nullptr,  // Parent
			nullptr,  // Menu
			GetModuleHandle(nullptr),
			this
		);
		
		if (!m_hwnd) 
		{
			return false;
		}

		s_windowMap[m_hwnd] = this;

		if (m_config.IsVisible)
		{
			Show();
		}

		return true;
	}

	void Window::Destroy()
	{
		if (m_hwnd) 
		{
			s_windowMap.erase(m_hwnd);
			::DestroyWindow(m_hwnd);
			m_hwnd = nullptr;
		}
	}

	void Window::Show()
	{
		if (m_hwnd)
		{
			ShowWindow(m_hwnd, SW_SHOW);
			UpdateWindow(m_hwnd);
		}
	}

	void Window::Hide()
	{
		if (m_hwnd)
		{
			ShowWindow(m_hwnd, SW_HIDE);
		}
	}

	void Window::Update()
	{
		MSG msg{};
		while (PeekMessageW(&msg, m_hwnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	void Window::SetTitle(const std::string& title)
	{
		m_config.Title = title;

		if (m_hwnd)
		{
			SetWindowTextW(m_hwnd, Utils::ToWideStr(title).c_str());
		}
	}

	void Window::SetSize(i32 width, i32 height)
	{
		m_config.Width  = width;
		m_config.Height = height;
		m_prevSize = m_currentSize = { width, height };

		if (m_hwnd)
		{
			DWORD style = GetWindowStyle();
			DWORD exStyle = GetExtendedWindowStyle();

			RECT windowRect = { 0, 0, width, height };
			::AdjustWindowRectEx(&windowRect, style, FALSE, exStyle);

			::SetWindowPos(m_hwnd, nullptr, 0, 0,
				windowRect.right - windowRect.left,
				windowRect.bottom - windowRect.top,
				SWP_NOMOVE | SWP_NOZORDER);
		}
	}

	void Window::SetPosition(i32 x, i32 y)
	{
		m_config.X = x;
		m_config.Y = y;

		if (m_hwnd)
		{
			::SetWindowPos(m_hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
	}

	bool Window::GetCanResize() const noexcept
	{
		return m_hwnd && m_config.IsResizable;
	}

	void Window::SetCanResize(bool canResize)
	{
		m_config.IsResizable = canResize;
		UpdateWindowStyle();
	}

	void Window::SetHasMaximizeButton(bool hasMaximizeButton)
	{
		m_config.HasMaximizeButton = hasMaximizeButton;
		UpdateWindowStyle();
	}

	void Window::SetHasMinimizeButton(bool hasMinimizeButton)
	{
		m_config.HasMinimizeButton = hasMinimizeButton;
		UpdateWindowStyle();
	}

	void Window::SetHasCloseButton(bool hasCloseButton)
	{
		m_config.HasCloseButton = hasCloseButton;
		if (m_hwnd) 
		{
			HMENU system_menu = ::GetSystemMenu(m_hwnd, FALSE);
			::EnableMenuItem(system_menu, SC_CLOSE, 
				MF_BYCOMMAND | (system_menu && hasCloseButton) ? MF_ENABLED : MF_GRAYED);
		}
	}
	
	void Window::RemoveAllEventListeners()
	{
		m_eventListeners.clear();
	}
	
	void Window::ClearPendingEvents()
	{
		s_pendingEvents.clear();
	}

	LRESULT Window::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:
		{
			DispatchEvent(CloseEvent{ hwnd });
			break;
		}

		case WM_ENTERSIZEMOVE:
		{
			HandleResizeTracking();
			break;
		}

		case WM_EXITSIZEMOVE: 
		{
			EndResizeTracking();
			break;
		}

		case WM_SIZE:
		{
			const i32 width = LOWORD(lParam);
			const i32 height = HIWORD(lParam);
			m_config.Width = width;
			m_config.Height = height;
			m_currentSize = { width, height };

			// Handle minimize/maximize/restore (but not regular resize)
			if (wParam == SIZE_MINIMIZED)
			{
				MinimizeEvent event{ hwnd };
				DispatchEvent(event);
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				DispatchEvent(MaximizeEvent{ hwnd });
				// For maximize, also fire resize immediately since it's not a drag operation
				if (m_currentSize != m_prevSize)
				{
					DispatchEvent(ResizeEvent{ hwnd, width, height });
					m_prevSize = m_currentSize;
				}
			}
			else if (wParam == SIZE_RESTORED && !m_isResizing)
			{
				DispatchEvent(RestoreEvent{ hwnd });
				// For restore, also fire resize immediately
				if (m_currentSize != m_prevSize)
				{
					DispatchEvent(ResizeEvent{ hwnd, width, height });
					m_prevSize = m_currentSize;
				}
			}
			break;
		}
		
		case WM_MOVE:
		{
			const i32 xPos = (i32)(short)LOWORD(lParam);   // horizontal position 
			const i32 yPos = (i32)(short)HIWORD(lParam);   // vertical position

			m_config.X = xPos;
			m_config.Y = yPos;

			DispatchEvent(MoveEvent{ hwnd, xPos, yPos });
			break;
		}

		case WM_KEYDOWN:
		case WM_KEYUP: 
		{
			bool IsDown = (msg == WM_KEYDOWN);
			bool IsRepeat = (lParam & 0x40000000) != 0;
			DispatchEvent(KeyEvent{ hwnd, static_cast<i32>(wParam), IsRepeat, IsDown });
			break;
		}

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		{
			const bool IsDown = (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN);
			i32 button = 0;
			
			if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
			{
				button = 1;
			}
			else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
			{
				button = 2;
			}

			DispatchEvent(MouseButtonEvent{ hwnd, LOWORD(lParam), HIWORD(lParam), button, IsDown });
			break;
		}

		case WM_MOUSEMOVE: 
		{
			DispatchEvent(MouseMoveEvent{ hwnd, LOWORD(lParam), HIWORD(lParam) });
			break;
		}

		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
		{
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			::ScreenToClient(hwnd, &pt);
			
			i32 delta = GET_WHEEL_DELTA_WPARAM(wParam) / 120;
			DispatchEvent(
				MouseWheelEvent
				{ 
					hwnd, pt.x, pt.y, delta, 
					(msg == WM_MOUSEWHEEL) 
						? MouseWheelEvent::WheelType:: Vertical 
						: MouseWheelEvent::WheelType::Horizontal 
				});
			break;
		}
		}

		return ::DefWindowProcW(hwnd, msg, wParam, lParam);
	}

	LRESULT Window::StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		Window* window{ nullptr };

		if (msg == WM_CREATE)
		{
			CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			window = static_cast<Window*>(cs->lpCreateParams);
			s_windowMap[hwnd] = window;
		}
		else
		{
			auto it = s_windowMap.find(hwnd);
			if (it != s_windowMap.end())
			{
				window = it->second;
			}
		}

		return window 
			? window->WindowProc(hwnd, msg, wParam, lParam) 
			: ::DefWindowProcW(hwnd, msg, wParam, lParam);
	}

	void Window::RegisterWindowClass()
	{
		if (s_isWindowClassRegistered)
		{
			return;
		}

		WNDCLASSEXW wc   = {};
		wc.cbSize        = sizeof(WNDCLASSEXW);
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = ::Ryu::Window::Window::StaticWindowProc;
		wc.hInstance     = ::GetModuleHandleW(nullptr);
		wc.hCursor       = ::LoadCursorW(nullptr, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszClassName = s_className;

		if (!RegisterClassExW(&wc)) 
		{
			throw std::runtime_error("Failed to register window class");
		}

		s_isWindowClassRegistered = true;
	}

	void Window::UpdateWindowStyle()
	{
		if (!m_hwnd)
		{
			return;
		}

		DWORD style = GetWindowStyle();
		DWORD exStyle = GetExtendedWindowStyle();

		::SetWindowLongPtrW(m_hwnd, GWL_STYLE, style);
		::SetWindowLongPtrW(m_hwnd, GWL_EXSTYLE, exStyle);

		::SetWindowPos(m_hwnd, nullptr, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

		::UpdateWindow(m_hwnd);
	}

	DWORD Window::GetWindowStyle() const
	{
		DWORD style = WS_OVERLAPPEDWINDOW;

		if (!m_config.IsResizable) 
		{
			style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
		}

		if (!m_config.HasMinimizeButton) 
		{
			style &= ~WS_MINIMIZEBOX;
		}

		if (!m_config.HasMaximizeButton)
		{
			style &= ~WS_MAXIMIZEBOX;
		}

		return style;
	}

	DWORD Window::GetExtendedWindowStyle() const
	{
		return WS_EX_APPWINDOW;
	}

	void Window::DispatchEvent(WindowEvent event)
	{
		s_pendingEvents.push_back(event);

		for (const auto& listener : m_eventListeners)
		{
			listener(event);
		}
	}

	void Window::HandleResizeTracking()
	{
		m_isResizing = true;
	}

	void Window::EndResizeTracking()
	{
		if (!m_isResizing)
		{
			return;
		}

		m_isResizing = false;

		// Only fire resize event if size changed
		if (m_currentSize != m_prevSize)
		{
			DispatchEvent(ResizeEvent{ m_hwnd, m_currentSize.first, m_currentSize.second });
			m_prevSize = m_currentSize;
		}
	}
	
	bool Window::GetIsVisible() const noexcept
	{
		return m_hwnd && ::IsWindowVisible(m_hwnd);
	}
	
	bool Window::GetIsMaximized() const noexcept
	{
		return m_hwnd && ::IsZoomed(m_hwnd);
	}
	
	bool Window::GetIsMinimized() const noexcept
	{
		return m_hwnd && ::IsIconic(m_hwnd);
	}
	
	bool Window::GetIsResizable() const noexcept
	{
		return false;
	}
}

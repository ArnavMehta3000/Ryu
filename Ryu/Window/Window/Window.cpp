#include "Window/Window.h"
#include "Common/Assert.h"
#include "Utils/StringConv.h"
#include "Config/CmdLine.h"
#include "Profiling/Profiling.h"
#include <stdexcept>
#include <dwmapi.h>

namespace Ryu::Window
{
	static Config::CVar<i32> cv_winWidth("Wnd.Width", -1, "Width of the window.", Config::CVarFlags::ReadOnly);
	static Config::CVar<i32> cv_winHeight("Wnd.Height", -1, "Height of the window.", Config::CVarFlags::ReadOnly);
	static Config::CVar<i32> cv_winPosX("Wnd.PosX", CW_USEDEFAULT, "X-Position of the window.", Config::CVarFlags::ReadOnly);
	static Config::CVar<i32> cv_winPosY("Wnd.PosY", CW_USEDEFAULT, "Y-Position of the window.", Config::CVarFlags::ReadOnly);


	namespace Internal
	{
		static const char* g_windowNotCreatedError = "Window not created. Call Window::Create() first.";
		static const char* g_windowAlreadyCreatedError = "Trying to create already created window.";
	}

	Window::Window(const Window::Config& config)
		: m_config(config)
		, m_prevSize(config.WindowSize[0], config.WindowSize[1])
		, m_currentSize(config.WindowSize[0], config.WindowSize[1])
	{
		RYU_PROFILE_SCOPE();
		::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		RegisterWindowClass();
	}

	Window::~Window()
	{
		RYU_PROFILE_SCOPE();
		Destroy();
	}

	bool Window::Create()
	{
		RYU_PROFILE_SCOPE();
		RYU_ASSERT(!m_hwnd, Internal::g_windowAlreadyCreatedError);

		if (m_hwnd)
		{
			return true;
		}

		DWORD style   = GetWindowStyle();
		DWORD exStyle = GetExtendedWindowStyle();

		// Update dimensions if they are passed in by the user
		const i32 winWidth = cv_winWidth.Get();
		if (winWidth != -1)
		{
			m_config.WindowSize[0] = winWidth;
		}

		const i32 winHeight = cv_winHeight.Get();
		if (winHeight != -1)
		{
			m_config.WindowSize[1] = winHeight;
		}

		RECT windowRect{ 0, 0, static_cast<LONG>(m_config.WindowSize[0]), static_cast<LONG>(m_config.WindowSize[1]) };
		::AdjustWindowRectEx(&windowRect, style, FALSE, exStyle);

		const i32 windowWidth  = windowRect.right - windowRect.left;
		const i32 windowHeight = windowRect.bottom - windowRect.top;

		const std::wstring windowTitle = Utils::ToWideStr(m_config.Title);

		// Check if custom position is passed in
		const i32 winPosX = cv_winPosX.Get();
		const i32 winPosY = cv_winPosY.Get();

		if (winPosX != CW_USEDEFAULT)
		{
			m_config.WindowPos[0] = winPosX;
		}

		if (winPosY != CW_USEDEFAULT)
		{
			m_config.WindowPos[1] = winPosY;
		}

		// If no position arguments are passed in, then assume that the user may have set the position via the config struct
		// If the config position is also `CW_USEDEFAULT`, then we center the window

		// Center on screen
		if (m_config.WindowPos[0] == CW_USEDEFAULT && m_config.WindowPos[1] == CW_USEDEFAULT)
		{
			const HDC screenDC = ::GetDC(nullptr);
			m_config.WindowPos =
			{
				::GetDeviceCaps(screenDC, HORZRES) / 2 - windowWidth / 2,
				::GetDeviceCaps(screenDC, VERTRES) / 2 - windowHeight / 2
			};
			::ReleaseDC(nullptr, screenDC);
		}

		m_hwnd = ::CreateWindowExW(
			exStyle,
			s_className,
			windowTitle.c_str(),
			style,
			m_config.WindowPos[0], m_config.WindowPos[1],
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
		m_input.Initialize(m_hwnd);

		if (m_config.IsVisible)
		{
			Show();
		}

		m_shouldClose = false;  // Set to true when window is closed
		return true;
	}

	void Window::Destroy()
	{
		RYU_PROFILE_SCOPE();
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
		if (m_hwnd)
		{
			m_input.Shutdown();
			s_windowMap.erase(m_hwnd);
			::DestroyWindow(m_hwnd);
			m_hwnd = nullptr;
		}
	}

	void Window::Show() const
	{
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
		if (m_hwnd)
		{
			ShowWindow(m_hwnd, SW_SHOW);
			UpdateWindow(m_hwnd);
		}
	}

	void Window::Hide() const
	{
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
		if (m_hwnd)
		{
			::ShowWindow(m_hwnd, SW_HIDE);
		}
	}

	void Window::Update()
	{
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
		if (!m_hwnd)
		{
			return;
		}

		RYU_PROFILE_SCOPE();
		m_input.UpdateStates();

		MSG msg{};
		while (PeekMessageW(&msg, m_hwnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	void Window::SetTitle(const std::string& title)
	{
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
		m_config.Title = title;

		if (m_hwnd)
		{
			SetWindowTextW(m_hwnd, Utils::ToWideStr(title).c_str());
		}
	}

	void Window::SetSize(i32 width, i32 height)
	{
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);

		m_config.WindowSize = { width, height };
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
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);

		m_config.WindowPos = { x, y };
		if (m_hwnd)
		{
			::SetWindowPos(m_hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
	}

	bool Window::GetCanResize() const noexcept
	{
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
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
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
		m_config.HasCloseButton = hasCloseButton;
		if (m_hwnd)
		{
			if (HMENU system_menu = ::GetSystemMenu(m_hwnd, FALSE))
			{
				::EnableMenuItem(system_menu, SC_CLOSE,
					MF_BYCOMMAND | ((system_menu && hasCloseButton) ? MF_ENABLED : MF_GRAYED));
			}
		}
	}

	void Window::SetIsDarkMode(bool isDarkMode) const
	{
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
		if (m_hwnd)
		{
			BOOL enable = isDarkMode;

			::DwmSetWindowAttribute(m_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &enable, sizeof(enable));
			::SetWindowPos(m_hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
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

		// Pass messages to window if valid
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
		RYU_PROFILE_SCOPE();

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
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
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
			Emit(ResizeEvent(m_hwnd, m_currentSize.first, m_currentSize.second));
			m_prevSize = m_currentSize;
		}
	}

	bool Window::GetIsVisible() const noexcept
	{
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
		return m_hwnd && ::IsWindowVisible(m_hwnd);
	}

	bool Window::GetIsMaximized() const noexcept
	{
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
		return m_hwnd && ::IsZoomed(m_hwnd);
	}

	bool Window::GetIsMinimized() const noexcept
	{
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
		return m_hwnd && ::IsIconic(m_hwnd);
	}

	bool Window::GetIsResizable() const noexcept
	{
		return false;
	}
}

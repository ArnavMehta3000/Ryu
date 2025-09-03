#include "Window/Window.h"
#include "Common/Assert.h"
#include "Utils/StringConv.h"
#include "Config/CmdLine.h"
#include "Profiling/Profiling.h"
#include <stdexcept>
#include <system_error>
#include <dwmapi.h>

namespace Ryu::Window
{
	static Config::CVar<i32> cv_winWidth("Wnd.Width", -1, "Width of the window.", Config::CVarFlags::ReadOnly);
	static Config::CVar<i32> cv_winHeight("Wnd.Height", -1, "Height of the window.", Config::CVarFlags::ReadOnly);
	static Config::CVar<i32> cv_winPosX("Wnd.PosX", CW_USEDEFAULT, "X-Position of the window.", Config::CVarFlags::ReadOnly);
	static Config::CVar<i32> cv_winPosY("Wnd.PosY", CW_USEDEFAULT, "Y-Position of the window.", Config::CVarFlags::ReadOnly);

	namespace Internal
	{
		static constexpr auto g_windowNotCreatedError     = "Window not created. Call Window::Create() first.";
		static constexpr auto g_windowAlreadyCreatedError = "Trying to create already created window.";

		// Updates the window size from the cvars (if needed)
		void UpdateWindowDimensions(WindowSizePos& current)
		{
			const i32 winWidth = cv_winWidth.Get();
			const i32 winHeight = cv_winHeight.Get();

			if (winWidth != -1)
			{
				current.Width = winWidth;
			}

			if (winHeight != -1)
			{
				current.Height = winHeight;
			}
		}

		// Updates the window position from the cvars (if needed)
		void UpdateWindowPosition(WindowSizePos& current, const WindowSizePos size)
		{
			const i32 winPosX = cv_winPosX.Get();
			const i32 winPosY = cv_winPosY.Get();

			if (winPosX != CW_USEDEFAULT)
			{
				current.X = winPosX;
			}

			if (winPosY != CW_USEDEFAULT)
			{
				current.Y = winPosY;
			}

			// If no position cmdline arguments are passed in, then assume that the user may have set the position via the config struct
			// If the config position is also `CW_USEDEFAULT`, then we center the window
			// Since Internal::UpdateWindowPosition will not change the position value if cmdline doesn't override it

			// Center on screen
			if (current.X == CW_USEDEFAULT && current.Y == CW_USEDEFAULT)
			{
				const HDC screenDC = ::GetDC(nullptr);
				current =
				{
					.X = ::GetDeviceCaps(screenDC, HORZRES) / 2 - size.Width / 2,
					.Y = ::GetDeviceCaps(screenDC, VERTRES) / 2 - size.Height / 2
				};
				::ReleaseDC(nullptr, screenDC);
			}
		}

		void SetWindowDarkTheme(HWND hWnd, bool isDarkMode)
		{
			BOOL enable = isDarkMode;
			::DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &enable, sizeof(enable));
			::SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
	}

	Window::Window(const Window::Config& config)
		: m_config(config)
		, m_prevSize({ config.WindowSize.Width, config.WindowSize.Height })
		, m_currentSize({ config.WindowSize.Width, config.WindowSize.Height })
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

		// Get style flags from config/window type
		const DWORD style = static_cast<DWORD>(GetStyleInternal());

		// Update dimensions if they are passed in by the user
		Internal::UpdateWindowDimensions(m_config.WindowSize);

		// Calculate new window size based on style
		RECT windowRect{ 0, 0, static_cast<LONG>(m_config.WindowSize.Width), static_cast<LONG>(m_config.WindowSize.Height) };
		::AdjustWindowRectEx(&windowRect, style, FALSE, 0);

		const i32 windowWidth  = windowRect.right - windowRect.left;
		const i32 windowHeight = windowRect.bottom - windowRect.top;

		// Update position, centre based on screen size if not set
		Internal::UpdateWindowPosition(m_config.WindowPos, { windowWidth, windowHeight });		

		const std::wstring windowTitle = Utils::ToWideStr(m_config.Title);
		m_hwnd = ::CreateWindowExW(
			0,
			s_className,
			windowTitle.c_str(),
			style,
			m_config.WindowPos.X, m_config.WindowPos.Y,
			windowWidth, windowHeight,
			nullptr,  // Parent
			nullptr,  // Menu
			::GetModuleHandle(nullptr),
			this
		);

		if (!m_hwnd)
		{
			return false;
		}

		s_windowMap[m_hwnd] = this;
		m_input.Initialize(m_hwnd);

		ToggleBorderless();
		Internal::SetWindowDarkTheme(m_hwnd, true);

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

	void Window::SetWindowType(WindowType newType)
	{
		m_config.Type = newType;
		ToggleBorderless();
	}

	void Window::SetTitle(const std::string& newTitle)
	{
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
		m_config.Title = newTitle;

		if (m_hwnd)
		{
			SetWindowTextW(m_hwnd, Utils::ToWideStr(newTitle).c_str());
		}
	}

	void Window::SetSize(i32 width, i32 height)
	{
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);

		m_config.WindowSize = { width, height };
		m_prevSize = m_currentSize = { width, height };

		if (m_hwnd)
		{
			RECT windowRect = { 0, 0, width, height };
			::AdjustWindowRectEx(&windowRect, std::to_underlying(GetStyleInternal()), FALSE, 0);

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
			throw std::system_error(
				std::error_code(::GetLastError(), std::system_category()),
				"Failed to register window class");
		}

		s_isWindowClassRegistered = true;
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
			Emit(ResizeEvent(m_hwnd, m_currentSize.Width, m_currentSize.Height));
			m_prevSize = m_currentSize;
		}
	}

	bool Window::IsCompositionEnabled() const
	{
		BOOL compositionEnabled = FALSE;
		bool success = ::DwmIsCompositionEnabled(&compositionEnabled) == S_OK;
		return success && compositionEnabled;
	}

	void Window::ToggleBorderless()
	{
		Internal::Style newStyle = GetStyleInternal();
		Internal::Style oldStyle = static_cast<Internal::Style>(::GetWindowLongPtrW(m_hwnd, GWL_STYLE));

		if (newStyle != oldStyle)
		{
			::SetWindowLongPtrW(m_hwnd, GWL_STYLE , static_cast<LONG>(newStyle));

			// When switching between borderless and windowed, restore appropriate shadow state
			if (IsCompositionEnabled())
			{
				const bool enabled = newStyle!= Internal::Style::Windowed;
				static const MARGINS shadow_state[2]{ { 0,0,0,0 },{ 1,1,1,1 } };
				::DwmExtendFrameIntoClientArea(m_hwnd, &shadow_state[enabled]);
			}

			// Redraw
			::SetWindowPos(m_hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
			Show();
		}
	}

	Internal::Style Window::GetStyleInternal() const
	{
		if (m_config.Type == WindowType::Borderless)
		{
			return IsCompositionEnabled() ? Internal::Style::AeroBorderless : Internal::Style::BasicBorderless;
		}

		return Internal::Style::Windowed;
	}

	bool Window::IsVisible() const noexcept
	{
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
		return m_hwnd && ::IsWindowVisible(m_hwnd);
	}

	bool Window::IsMaximized() const noexcept
	{
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
		
		WINDOWPLACEMENT placement{};
		if (!::GetWindowPlacement(m_hwnd, &placement))
		{
			return false;
		}

		return placement.showCmd == SW_MAXIMIZE;
	}

	bool Window::IsMinimized() const noexcept
	{
		RYU_ASSERT(m_hwnd, Internal::g_windowNotCreatedError);
		return m_hwnd && ::IsIconic(m_hwnd);
	}
	
	bool Window::GetIsResizable() const noexcept
	{
		return false;
	}
}

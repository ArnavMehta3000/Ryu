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
		RYU_CORE_LOG_DEBUG(Core, "Toggled window to {}", (wPos.showCmd == SW_MAXIMIZE) ? "normal" : "maximized");
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
			
			if (msg.message == WM_TIMER)
			{
				RedrawWindow();
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

		case WM_NCCREATE:
		{
			OnNonClientCreate();
			return TRUE;
		}

		case WM_NCACTIVATE:
		{
			OnNonClientActivate(LOWORD(wParam) != WA_INACTIVE);
			return 0;
		}

		case WM_NCPAINT:
		{
			OnNonClientPaint((HRGN)wParam);
			return 0;
		}

		case WM_PAINT:
		{
			OnPaint();
			return 0;
		}

		case WM_CLOSE: FALLTHROUGH;
		case WM_DESTROY:
		{
			::KillTimer(hWnd, Caption::TIMER_ID);
			m_isOpen = false;
			return 0;
		}

		case WM_NCLBUTTONDOWN:
		{
			OnNonClientLeftMouseButtonDown();
			break;
		}

		case WM_NCLBUTTONDBLCLK:
		{
			// Double clicked non-client region, toggle maximize if resizeable
			if (m_config.Type == Type::Resizeable)
			{
				ToggleMaximize();
				return 0;
			}
			break;
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
		}

		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	
	void Window::RedrawWindow()
	{
		::SetWindowPos(m_hWnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_DRAWFRAME | SWP_FRAMECHANGED); // reset window
		::SendMessage(m_hWnd, WM_PAINT, 0, 0);
	}
	
	void Window::OnNonClientCreate()
	{
		::SetUserObjectInformation(m_hWnd, UOI_TIMERPROC_EXCEPTION_SUPPRESSION, FALSE, sizeof(BOOL));
		::SetTimer(m_hWnd, Caption::TIMER_ID, 100, (TIMERPROC)NULL);
		::SetWindowTheme(m_hWnd, L"", L"");
		ModifyClassStyle(0, CS_DROPSHADOW);

		Caption::AddButton(Caption::Button(L"X", Caption::CLOSE));
		if (m_config.Type != Type::Static)
		{
			Caption::AddButton(Caption::Button(L"🗖", Caption::MAXIMIZE));
		}
		Caption::AddButton(Caption::Button(L"🗕", Caption::MINIMIZE));
	}	

	void Window::OnNonClientPaint(HRGN region)
	{
#ifndef DCX_USESTYLE
#define RYU_DEFINED_USE_STYLE
#define DCX_USESTYLE 0x00010000
#endif
		HDC hdc = ::GetDCEx(m_hWnd, region, DCX_WINDOW | DCX_INTERSECTRGN | DCX_USESTYLE);

		RECT rect;
		::GetWindowRect(m_hWnd, &rect);
		SIZE size = SIZE{ rect.right - rect.left, rect.bottom - rect.top };

		const HBITMAP hbmMem = ::CreateCompatibleBitmap(hdc, size.cx, size.cy);
		const HANDLE hOld    = ::SelectObject(hdc, hbmMem);
		HBRUSH brush         = ::CreateSolidBrush(RGB(46, 46, 46));
		const RECT newRect   = RECT{ 0, 0, size.cx, size.cy };

		FillRect(hdc, &newRect, brush);

		if (m_activated && !IsMaximized()) 
		{
			brush = ::CreateSolidBrush(RGB(0, 100, 150));
			::FrameRect(hdc, &newRect, brush);
		}

		PaintCaption(hdc);
		::DeleteObject(brush);

		::BitBlt(hdc, 0, 0, size.cx, size.cy, hdc, 0, 0, SRCCOPY);

		::SelectObject(hdc, hOld);
		::DeleteObject(hbmMem);
		::ReleaseDC(m_hWnd, hdc);

#ifdef RYU_DEFINED_USE_STYLE
#undef RYU_DEFINED_USE_STYLE
#undef DCX_USESTYLE
#endif

	}

	void Window::OnNonClientActivate(bool active)
	{
		m_activated = active;
	}
	
	void Window::PaintCaption(HDC hdc)
	{
		RECT rect;
		::GetWindowRect(m_hWnd, &rect);

		SIZE size = SIZE{ rect.right - rect.left, rect.bottom - rect.top };

		// Draw title
		if (Caption::CanShowTitle()) 
		{
			rect = RECT{ 0, 0, size.cx, 30 };

			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, m_activated ? RGB(255, 255, 255) : RGB(92, 92, 92));

			DrawText(hdc, m_title.c_str(), u32(wcslen(m_title.c_str())), &rect, u32(DT_SINGLELINE | DT_VCENTER | DT_CENTER));
		}


		POINT pt;
		u32 spacing = 0;
		::GetCursorPos(&pt);
		::GetWindowRect(m_hWnd, &rect);

		pt.x -= rect.left;
		pt.y -= rect.top;

		
		// Draw captions buttons
		for (Ryu::Caption::Button& button : Caption::GetButtons())
		{
			spacing += button.Width;
			button.Rect = RECT
			{
				size.cx - LONG(spacing),
				0, 
				size.cx - LONG(spacing)+ button.Width,
				30
			};

			if (button.Rect.left < pt.x
				&& button.Rect.right > pt.x
				&& button.Rect.top < pt.y
				&& button.Rect.bottom > pt.y) 
			{

				HBRUSH brush = ::CreateSolidBrush(RGB(92, 92, 92));
				::FillRect(hdc, &button.Rect, brush);
				::DeleteObject(brush);
			}

			if (button.Text.compare(L"🗖") == 0 && IsMaximized())
{
				button.Text = L"🗗";
			}
			else if (button.Text.compare(L"🗗") == 0 && !IsMaximized())
			{
				button.Text = L"🗖";
			}

			DrawText(hdc, button.Text.c_str(), u32(wcslen(button.Text.c_str())), &button.Rect, u32(DT_SINGLELINE | DT_VCENTER | DT_CENTER));
		}
	}
	
	void Window::OnNonClientLeftMouseButtonDown()
	{
		POINT pt;
		RECT rect;
		::GetCursorPos(&pt);
		::GetWindowRect(m_hWnd, &rect);

		pt.x -= rect.left;
		pt.y -= rect.top;


		for (Ryu::Caption::Button& button : Caption::GetButtons())
		{
			if (button.Rect.left < pt.x
				&& button.Rect.right > pt.x
				&& button.Rect.top < pt.y
				&& button.Rect.bottom > pt.y)
			{

				switch (button.Command) 
				{
				case Caption::CLOSE:    { ::SendMessage(m_hWnd, WM_CLOSE, 0, 0); } break;
				case Caption::MINIMIZE: { ::ShowWindow(m_hWnd, SW_MINIMIZE);     } break;
				case Caption::MAXIMIZE: { ToggleMaximize();                      } break;
				}
			}

		}
	}
	
	void Window::OnGetMinMaxInfo(MINMAXINFO* minmax)
	{
		RECT WorkArea; ::SystemParametersInfo(SPI_GETWORKAREA, 0, &WorkArea, 0);
		minmax->ptMaxSize.x      = (WorkArea.right - WorkArea.left);
		minmax->ptMaxSize.y      = (WorkArea.bottom - WorkArea.top);
		minmax->ptMaxPosition.x  = WorkArea.left;
		minmax->ptMaxPosition.y  = WorkArea.top;
		minmax->ptMinTrackSize.x = 400;
		minmax->ptMinTrackSize.y = 300;
	}
	
	void Window::OnExitSizeMove()
	{
		RECT rect;
		::GetWindowRect(m_hWnd, &rect);
		RECT WorkArea;
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &WorkArea, 0);
		if (rect.top < WorkArea.top + 10 && !IsMaximized())
		{
			ToggleMaximize();
		}

		// Capture window size
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
	
	void Window::OnPaint()
	{
		PAINTSTRUCT ps;
		RECT rc;

		const HDC hdc = ::BeginPaint(m_hWnd, &ps);
		::GetClientRect(m_hWnd, &rc);

		const HBRUSH brush = ::CreateSolidBrush(RGB(36, 36, 36));

		::FillRect(hdc, &rc, brush);
		::DeleteObject(brush);
		::EndPaint(m_hWnd, &ps);
	}
}
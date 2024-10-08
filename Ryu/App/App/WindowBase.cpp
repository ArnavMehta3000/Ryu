#include "WindowBase.h"
#include "Utils/MessageBox.h"

namespace Ryu::App
{
	namespace
	{
		LRESULT CALLBACK SetupMessageHandler(MAYBE_UNUSED HWND hWnd, MAYBE_UNUSED UINT uMsg, MAYBE_UNUSED WPARAM wParam, MAYBE_UNUSED LPARAM lParam)
		{
			WindowBase* object{ nullptr };

			if (uMsg == WM_NCCREATE)
			{
				const LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
				object = static_cast<WindowBase*>(lpcs->lpCreateParams);

				// Set user data to be the sub object pointer
				::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(object));

				// Set the WNDPROC
				::SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Ryu::App::WindowBase::AssignMessageHandler));
			}

			if (object)
			{
				return object->MessageHandler(hWnd, uMsg, wParam, lParam);
			}

			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

	WindowBase::WindowBase(HINSTANCE hInst, const std::wstring& className)
		: m_hInstance(hInst)
		, m_className(className)
	{
		RegisterWindowClass();
	}

	LRESULT WindowBase::AssignMessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WindowBase* const object = reinterpret_cast<WindowBase*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		return object->MessageHandler(hWnd, uMsg, wParam, lParam);
	}

	void WindowBase::RegisterWindowClass()
	{
		// Check if class with the same name already exists
		{
			WNDCLASSEX wc{};
			if (GetClassInfoEx(m_hInstance, m_className.c_str(), &wc) != 0)
			{
				return;  // Class already registered
			}
		}

		WNDCLASSEX wc{};
		ZeroMemory(&wc, sizeof(WNDCLASSEX));

		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
		wc.lpfnWndProc   = &SetupMessageHandler;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = m_hInstance;
		wc.hIcon         = LoadIcon(nullptr, IDI_WINLOGO);
		wc.hIconSm       = wc.hIcon;
		wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName  = nullptr;
		wc.lpszClassName = m_className.c_str();

		if (RegisterClassEx(&wc) == 0)
		{
			Utils::MessageBoxDesc desc
			{
				.Window = nullptr,
				.Title = "Window Creation Error",
				.Text = "Failed to register window class GetLastError() = " + std::to_string(GetLastError()),
				.Flags = { Utils::MessagBoxButton::Ok, Utils::MessageBoxIcon::Error }
			};

			Utils::ShowMessageBox(desc);
		}
	}
}

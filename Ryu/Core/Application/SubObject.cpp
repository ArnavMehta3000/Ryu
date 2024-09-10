#include "SubObject.h"
#include <Core/Log/Log.h>
#include <Core/Log/Logger.h>
#include <Core/Utils/StringConv.h>

namespace Ryu
{
	namespace
	{
		LRESULT CALLBACK SetupMessageHandler(MAYBE_UNUSED HWND hWnd, MAYBE_UNUSED UINT uMsg, MAYBE_UNUSED WPARAM wParam, MAYBE_UNUSED LPARAM lParam)
		{
			Ryu::SubObject* object{ nullptr };

			if (uMsg == WM_NCCREATE)
			{
				const LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
				object = static_cast<Ryu::SubObject*>(lpcs->lpCreateParams);
				
				// Set user data to be the sub object pointer
				::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(object));

				// Set the WNDPROC
				::SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Ryu::SubObject::AssignMessageHandler));
			}

			if (object)
			{
				return object->MessageHandler(hWnd, uMsg, wParam, lParam);
			}
				
			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

	SubObject::SubObject(const std::wstring& className, const std::wstring& title, HINSTANCE hInst, HICON icon)
		: m_className(className)
		, m_title(title)
		, m_hInstance(hInst)
		, m_icon(icon)
	{
	}

	bool SubObject::RegisterNewClass()
	{
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
		wc.hbrBackground = (HBRUSH)(::CreateSolidBrush(RGB(46, 46, 46)));;
		wc.lpszMenuName  = nullptr;
		wc.lpszClassName = m_className.c_str();

		RYU_CORE_LOG_DEBUG(Core, "Registering window class: {}", ToNarrowStr(m_className));
		return ::RegisterClassEx(&wc) != 0;
	}

	LRESULT SubObject::AssignMessageHandler(MAYBE_UNUSED HWND hWnd, MAYBE_UNUSED UINT uMsg, MAYBE_UNUSED WPARAM wParam, MAYBE_UNUSED LPARAM lParam)
	{
		SubObject* const object = reinterpret_cast<SubObject*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		return object->MessageHandler(hWnd, uMsg, wParam, lParam);
	}
}
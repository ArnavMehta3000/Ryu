#pragma once
#include "Common/Common.h"
#include <Windows.h>
#include <string>

namespace Ryu::App
{
	class WindowBase
	{
	public:
		WindowBase(HINSTANCE hInst, const std::wstring& className);
		virtual ~WindowBase() = default;

		virtual LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
		static LRESULT CALLBACK AssignMessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		// Returns the window's HINSTANCE
		inline HINSTANCE GetHInstance() const { return m_hInstance; }

		// Returns the window's class name
		inline const std::wstring& GetBaseName() const { return m_className; }

	private:
		void RegisterWindowClass();

	private:
		HINSTANCE    m_hInstance;
		std::wstring m_className;
	};
}

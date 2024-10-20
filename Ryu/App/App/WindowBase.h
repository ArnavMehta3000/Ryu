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
		constexpr inline HINSTANCE GetHInstance() const noexcept { return m_hInstance; }
		constexpr inline HWND GetHWND() const noexcept { return m_hWnd; }
		constexpr inline u32 GetWidth() const noexcept { return m_width; }
		constexpr inline u32 GetHeight() const noexcept { return m_height; }

		// Returns the window's class name
		constexpr inline const std::wstring& GetBaseName() const { return m_className; }
		constexpr operator HWND() const noexcept { return m_hWnd; }

	private:
		void RegisterWindowClass();
	
	protected:
		HWND m_hWnd;
		u32 m_width;
		u32 m_height;

	private:
		HINSTANCE    m_hInstance;
		std::wstring m_className;
	};
}

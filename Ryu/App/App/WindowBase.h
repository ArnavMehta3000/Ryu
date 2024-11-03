#pragma once
#include "Common/Common.h"
#include <Windows.h>
#include <string>

namespace Ryu::App
{
	/**
	 * @brief Base class for all windows in the application
	 */
	class WindowBase
	{
	public:
		/**
		 * @brief Window constructor
		 * @param hInst HINSTANCE of the application
		 * @param className Win32 lass name of the window
		 */
		RYU_API WindowBase(HINSTANCE hInst, const std::wstring& className);
		
		/**
		 * @brief Virtual destructor
		 */
		virtual RYU_API ~WindowBase() = default;

		/**
		 * @brief Abstract function for handling window messages
		 * @param hWnd Handle to the window
		 * @param uMsg Window message
		 * @param wParam WPARAM
		 * @param lParam LPARAM
		 */
		virtual RYU_API LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

		/**
		 * @brief Static method to route window messages
		 * @param hWnd Handle to the window
		 * @param uMsg Window message
		 * @param wParam WPARAM
		 * @param lParam LPARAM
		 */
		static LRESULT CALLBACK AssignMessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		/**
		 * @brief Returns the HINSTANCE of the application
		 * @return HINSTANCE of the application
		 */
		constexpr inline RYU_API HINSTANCE GetHInstance() const noexcept { return m_hInstance; }

		/**
		 * @brief Returns the Win32 handle of the window
		 * @return Win32 handle (HWND)
		 */
		constexpr inline RYU_API HWND GetHWND() const noexcept { return m_hWnd; }

		/**
		 * @brief Returns the (client) width of the window
		 * @return Window width
		 */
		constexpr inline RYU_API u32 GetWidth() const noexcept { return m_width; }

		/**
		 * @brief Returns the (client) height of the window
		 * @return Window height
		 */
		constexpr inline RYU_API u32 GetHeight() const noexcept { return m_height; }

		/**
		 * @brief Returns the Win32 class name of the window
		 * @return Win32 class name
		 */
		constexpr inline RYU_API const std::wstring& GetBaseName() const { return m_className; }

		/**
		 * @brief Operator to get the Win32 handle of the window
		 * @return Win32 handle
		 */
		constexpr RYU_API operator HWND() const noexcept { return m_hWnd; }

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

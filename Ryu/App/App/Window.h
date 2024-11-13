#pragma once
#include "App/WindowBase.h"
#include "App/Events/IWindowEventListener.h"

namespace Ryu::App
{
	/**
	 * @brief Class that represents a Win32 window
	 */
	class Window : public WindowBase
	{
	public:
		/**
		 * @brief Construct a new Window object
		 * @param name Title of the window
		 * @param width Starting width
		 * @param height Starting height
		 * @param eventListener Pointer to window event listener
		 */
		RYU_API Window(const std::wstring& name, u32 width, u32 height, IWindowEventListener* eventListener = nullptr);

		/**
		 * @brief Window destructor
		 */
		RYU_API ~Window() = default;

		/**
		 * @brief Create and show the window
		 */
		RYU_API void Create();

		/**
		 * @brief Run Win32 message pump
		 */
		RYU_API void PumpMessages();


		/**
		 * @brief Show the window
		 */
		RYU_API void ShowWindow();

		/**
		 * @brief Override window event listener
		 * @param eventListener Pointer to new window event listener
		 */
		inline  RYU_API void SetEventListener(IWindowEventListener* eventListener) { m_eventListener = eventListener; }

		/**
		 * @brief Get window handle
		 * @return HWND
		 */
		inline  RYU_API operator HWND() const { return m_hWnd; }

	private:
		template <typename... Args>
		void SendEvent(Args&&... args)
		{
			if (m_eventListener)
			{
				m_eventListener->OnEvent(std::forward<Args>(args)...);
			}
		}

		void SetDarkTheme();
		LRESULT MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	private:
		IWindowEventListener* m_eventListener;		
		std::wstring m_name;
		bool m_minimized;
		bool m_maximized;
		bool m_resizing;
	};
}

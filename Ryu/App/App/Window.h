#pragma once
#include "App/WindowBase.h"
#include "App/Events/IWindowEventListener.h"

namespace Ryu::App
{
	class Window : public WindowBase
	{
	public:
		Window(const std::wstring& name, u32 width, u32 height, IWindowEventListener* eventListener = nullptr);
		~Window() = default;

		void Create();
		void PumpMessages();

		inline void SetEventListener(IWindowEventListener* eventListener) { m_eventListener = eventListener; }
		inline operator HWND() const { return m_hWnd; }

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

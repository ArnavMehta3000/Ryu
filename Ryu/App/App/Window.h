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
		NODISCARD inline HWND GetHandle() const { return m_hWnd; }
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
		HWND m_hWnd;
		IWindowEventListener* m_eventListener;
		u32 m_width;
		u32 m_height;
		std::wstring m_name;
	};
}

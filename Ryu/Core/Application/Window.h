#pragma once
#include <CoreDLLDefines.h>
#include <Core/StandardTypes.h>
#include <Core/ObjectMacros.h>
#include <Windows.h>
#include <functional>

namespace Ryu
{
	class RYU_API Window
	{
	public:
		using ResizeCallback = std::function<void(i32, i32)>;
		struct RYU_API Config
		{
			const wchar_t* Title = L"Ryu Window";
			HICON Icon           = ::LoadIcon(NULL, IDI_WINLOGO);
			HCURSOR Cursor       = ::LoadCursor(NULL, IDC_ARROW);
			HINSTANCE Instance   = ::GetModuleHandle(NULL);
			i32 Width            = 1280;
			i32 Height           = 720;
			i32 MinWidth         = 380;
			i32 MinHeight        = 200;
			bool Windowed        = true;
			bool Borderless      = false;
		};

	public:
		RYU_DISABLE_COPY_AND_MOVE(Window);
		Window(Window::Config wndConfig = Window::Config());
		virtual ~Window();
		NODISCARD inline HWND GetHandle() const { return m_hWnd; }
		NODISCARD inline bool IsOpen() const { return m_isOpen; }
		NODISCARD inline bool HasFocus() const { return m_hasFocus; }
		NODISCARD bool Create();
		void Show();
		void PumpMessages();
		inline void SetResizeCallback(ResizeCallback callback) { m_resizeCallback = std::move(callback); }

	private:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		ResizeCallback m_resizeCallback;
		Window::Config m_config;
		HWND m_hWnd;
		bool m_isOpen;
		bool m_hasFocus;
	};
}
#pragma once
#include <CoreDLLDefines.h>
#include <Core/StandardTypes.h>
#include <Core/ObjectMacros.h>
#include <Core/Application/SubObject.h>
#include <Core/Application/Caption.h>
#include <functional>
#include <Uxtheme.h>
#pragma comment(lib,"uxtheme.lib")

namespace Ryu
{
	class RYU_API Window : public SubObject, public Caption
	{
		friend class Engine;

	public:
		enum class Type : DWORD
		{
			Static     = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX,
			Resizeable = WS_OVERLAPPEDWINDOW,
			PopUp      = WS_POPUP
		};

		using ResizeCallback = std::function<void(i32, i32)>;

		struct RYU_API Config
		{
			const wchar_t* Title = L"Ryu Window";
			HICON Icon           = ::LoadIcon(NULL, IDI_APPLICATION);
			HCURSOR Cursor       = ::LoadCursor(NULL, IDC_ARROW);
			HINSTANCE Instance   = ::GetModuleHandle(NULL);
			Window::Type Type    = Window::Type::Static;
			i32 Width            = 1280;
			i32 Height           = 720;
			i32 MinWidth         = 380;
			i32 MinHeight        = 200;
		};

	public:
		RYU_DISABLE_COPY_AND_MOVE(Window);
		explicit Window(Window::Config wndConfig = Window::Config());
		virtual ~Window();
		NODISCARD inline HWND GetHandle() const { return m_hWnd; }
		NODISCARD inline bool IsOpen() const { return m_isOpen; }
		NODISCARD inline const Window::Config& GetConfig() const { return m_config; }
		NODISCARD inline bool IsActivated() const { return m_activated; }
		virtual bool Init() override;
		void Show();
		void ToggleMaximize();
		void PumpMessages();
		bool IsMaximized();
		bool HasStyle(DWORD style); 
		void SetStyle(DWORD flagsToDisable, DWORD flagsToEnable);
		void SetStyleEx(DWORD flagsToDisable, DWORD flagsToEnable);
		void ModifyClassStyle(DWORD flagsToDisable, DWORD flagsToEnable);
		inline operator HWND() const { return m_hWnd; }

	protected:
		LRESULT MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		virtual void OnGetMinMaxInfo(MINMAXINFO* minmax);
		virtual void OnExitSizeMove();

	private:
		inline void SetResizeCallback(ResizeCallback callback) { m_resizeCallback = std::move(callback); }
	
	private:
		ResizeCallback m_resizeCallback;
		Window::Config m_config;
		HWND m_hWnd;
		bool m_isOpen;
		bool m_activated;
	};
}

#include "SplashScreen.h"
#include <memory>
namespace Ryu
{
	namespace SplashScreen
	{
		const Window::Config SplashWndConfig
		{
			.Title = L"RyuSplashScreen",
			.Type = Window::Type::PopUp,
			.Width = 800,
			.Height = 600,
		};

		std::unique_ptr<SplashWindow> g_splashWindow{ nullptr };

		RYU_API void Open()
		{
			g_splashWindow = std::make_unique<SplashWindow>();
		}

		RYU_API void Close()
		{
			g_splashWindow.reset();
		}

		RYU_API void AddMessage(std::string_view message)
		{
			
		}
	}

	SplashWindow::SplashWindow()
		: Window(SplashScreen::SplashWndConfig)
	{
		Window::Init();
		Window::Show();
	}

	LRESULT SplashWindow::MessageHandler(MAYBE_UNUSED HWND hWnd, MAYBE_UNUSED UINT uMsg, MAYBE_UNUSED WPARAM wParam, MAYBE_UNUSED LPARAM lParam)
	{
		return Window::MessageHandler(hWnd, uMsg, wParam, lParam);
	}
}
#pragma once	
#include <Core/Application/Window.h>

namespace Ryu
{
	namespace SplashScreen
	{
		RYU_API void Open();
		RYU_API void Close();
		RYU_API void AddMessage(std::string_view message);
	}

	class RYU_API SplashWindow : public Window
	{
	public:
		
		SplashWindow();
		~SplashWindow() = default;

		LRESULT MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	private:
		std::wstring m_msg;

	};
}
#pragma once
#include "App/Application.h"
#include "Engine/Game/GameModuleLoader.h"
#include "Logger/LogCategory.h"

namespace Ryu::Editor
{
	// Abstraction around the user application loaded via the game module
	class EditorApp : public App::App
	{
	public:
		RYU_LOG_DECLARE_CATEGORY(Editor);

		EditorApp(std::shared_ptr<Window::Window> window);

	private:
		static LRESULT CALLBACK EditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		bool OnInit() override final;
		void OnShutdown() override final;
		void OnTick(const Utils::TimeInfo& timeInfo);
		
		bool RouteWndProc() const;
		void InitImGui();
		void ShutdownImGui() const;

		bool LoadGameModule();

	private:
		Engine::GameModuleLoader m_gameModuleLoader;
		std::shared_ptr<App> m_userApp;
	};
}

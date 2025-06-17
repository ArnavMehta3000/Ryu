#pragma once
#include "App/Application.h"
#include "Engine/Game/GameModuleLoader.h"
#include "Logger/LogCategory.h"

namespace Ryu::Editor
{
	// Contains the user application
	class EditorApp : public App::App
	{
	public:
		RYU_LOG_DECLARE_CATEGORY(Editor);
		static LRESULT CALLBACK EditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		EditorApp(std::shared_ptr<Window::Window> window);

	private:
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

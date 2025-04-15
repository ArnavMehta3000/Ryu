#pragma once
#include "App/Application.h"
#include "Logger/LogCategory.h"

namespace Ryu::Editor
{
	class EditorApp : public App::Application
	{
	public:
		RYU_LOG_DECLARE_CATEGORY(Editor);
		static LRESULT CALLBACK EditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		bool OnInit() override final;
		void OnShutdown() override final;
		void OnTick(const Utils::TimeInfo& timeInfo);
		
		bool RouteWndProc() const;
		void InitImGui();
		void ShutdownImGui() const;
	};
}

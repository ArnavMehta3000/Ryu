#pragma once
#include <Core/Includes.h>
#include <Engine/Core/Application.h>

namespace Ryu::Editor
{
	class RYU_API EditorApp : public Application
	{
	public:
		EditorApp(HINSTANCE hInst);
		~EditorApp();

		bool OnInit() override final;
		void OnShutdown() override final;
		void OnRender(f32 dt) override final;
		static LRESULT CALLBACK EditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		bool RouteWndProc();
		void InitImGui();
		void ShutdownImGui();
	};
}
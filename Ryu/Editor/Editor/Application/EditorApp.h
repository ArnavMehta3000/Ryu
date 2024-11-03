#pragma once
#include "Engine/Runtime/Runtime.h"
#include "Logger/LogCategory.h"

namespace Ryu::Editor
{
	class EditorApp : public Engine::Runtime
	{
	public:
		RYU_LOG_CATEGORY(Editor);
		static LRESULT CALLBACK EditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		bool OnInit() override final;
		void OnShutdown() override final;
		void OnTick(f64 dt);
		
		bool RouteWndProc() const;
		void InitImGui();
		void ShutdownImGui() const;
	};
}

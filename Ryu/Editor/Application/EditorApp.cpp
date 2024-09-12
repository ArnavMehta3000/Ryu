#include "EditorApp.h"
#include <Editor/Internal/Log.h>
#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Ryu::Editor
{
	namespace
	{
		WNDPROC s_originalWndProc{ nullptr };
	}

	EditorApp::EditorApp(HINSTANCE hInst) 
		: Application(Window::Config 
			{ 
				.Title = L"Ryu Editor",
				.Instance = hInst,
				.Type = Window::Type::Resizeable,
				.Width = 1280,
				.Height = 720,
			})
	{
		RYU_EDITOR_TRACE("EditorApp constructed");
	}

	EditorApp::~EditorApp()
	{
		RYU_EDITOR_TRACE("EditorApp destructed");
	}

	bool EditorApp::OnInit()
	{
		if (!Application::OnInit())
		{
			return false;
		}

		if (!RouteWndProc())
		{
			RYU_EDITOR_FATAL("Failed to route WndProc for EditorApp");
		}

		return true;
	}

	bool EditorApp::RouteWndProc()
	{
		s_originalWndProc = (WNDPROC)::SetWindowLongPtr(GetWindow() , GWLP_WNDPROC, (LONG_PTR)&EditorApp::EditorWndProc);
		return s_originalWndProc != nullptr;
	}

	LRESULT EditorApp::EditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		std::ignore = ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return ::CallWindowProc(s_originalWndProc, hWnd, msg, wParam, lParam);
	}
}
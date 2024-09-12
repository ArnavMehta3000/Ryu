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

		InitImGui();
		
		return true;
	}

	void EditorApp::OnShutdown()
	{
		ShutdownImGui();
		Application::OnShutdown();
	}

	void EditorApp::OnRender(f32 dt)
	{
		Application::OnRender(dt);
	}

	bool EditorApp::RouteWndProc()
	{
		s_originalWndProc = (WNDPROC)::SetWindowLongPtr(GetWindow() , GWLP_WNDPROC, (LONG_PTR)&EditorApp::EditorWndProc);
		
		const bool success = s_originalWndProc != nullptr;
		if (success)
		{
			RYU_EDITOR_DEBUG("WndProc routed to EditorApp");
		}

		return success;
	}

	void EditorApp::InitImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

		ImGui_ImplWin32_Init(GetWindow());
		//ImGui_ImplDX11_Init(YOUR_D3D_DEVICE, YOUR_D3D_DEVICE_CONTEXT);
		RYU_EDITOR_DEBUG("ImGui initialized");
	}

	void EditorApp::ShutdownImGui()
	{
		//ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		RYU_EDITOR_DEBUG("ImGui shutdown");
	}

	LRESULT EditorApp::EditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		std::ignore = ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return ::CallWindowProc(s_originalWndProc, hWnd, msg, wParam, lParam);
	}
}
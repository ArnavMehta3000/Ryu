#include "EditorApp.h"
#include "Logger/Logger.h"
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_dx11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Ryu::Editor
{
	namespace
	{
		// Handle to the original application window procedure
		WNDPROC s_originalWndProc{ nullptr };
	}

	bool EditorApp::OnInit()
	{
		if (!RouteWndProc())
		{
			LOG_ERROR(RYU_USE_LOG_CATEGORY(Editor), "Failed to route Editor WndProc!");
			return false;
		}

		InitImGui();
		
		return true;
	}

	void EditorApp::OnShutdown()
	{
		ShutdownImGui();
	}

	void EditorApp::OnTick(MAYBE_UNUSED f64 dt)
	{
	}

	bool EditorApp::RouteWndProc() const
	{
		s_originalWndProc = (WNDPROC)::SetWindowLongPtr(GetWindow()->GetHWND() , GWLP_WNDPROC, (LONG_PTR)&EditorApp::EditorWndProc);
		
		const bool success = s_originalWndProc != nullptr;
		if (success)
		{
			LOG_TRACE(RYU_USE_LOG_CATEGORY(Editor), "WndProc routed to EditorApp");
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

		ImGui_ImplWin32_Init(GetWindow()->GetHWND());
		//ImGui_ImplDX11_Init(YOUR_D3D_DEVICE, YOUR_D3D_DEVICE_CONTEXT);
		LOG_TRACE(RYU_USE_LOG_CATEGORY(Editor), "ImGui initialized");
	}

	void EditorApp::ShutdownImGui() const
	{
		//ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		LOG_INFO(RYU_USE_LOG_CATEGORY(Editor), "ImGui shutdown");
	}

	LRESULT EditorApp::EditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		std::ignore = ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return ::CallWindowProc(s_originalWndProc, hWnd, msg, wParam, lParam);
	}
}

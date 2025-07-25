#include "EditorApp.h"
#include "App/Utils/PathManager.h"
#include "Game/IGameModule.h"
#include "Logger/Logger.h"
#include "Profiling/Profiling.h"
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Ryu::Editor
{
	RYU_LOG_DECLARE_CATEGORY(EditorApp);

	namespace
	{
		// Handle to the original application window procedure
		WNDPROC s_originalWndProc{ nullptr };
	}

	EditorApp::EditorApp(std::shared_ptr<Window::Window> window)
		: App::App(window)
	{
	}

	bool EditorApp::OnInit()
	{
		RYU_PROFILE_SCOPE();

		RYU_LOG_TRACE(LogEditorApp, "Initializing editor application");

		if (!RouteWndProc())
		{
			RYU_LOG_ERROR(LogEditorApp, "Failed to route Editor WndProc!");
			return false;
		}

		RYU_TODO("Init Imgui");
		//InitImGui();

		if (!LoadGameModule())
		{
			RYU_LOG_ERROR(LogEditorApp, "Failed to load game module!");
			return false;
		}

		// Init user application
		RYU_LOG_TRACE(LogEditorApp, "Initializing user application");
		return m_userApp->OnInit();
	}

	void EditorApp::OnShutdown()
	{
		RYU_PROFILE_SCOPE();

		RYU_LOG_TRACE(LogEditorApp, "Shutting down editor application");

		m_userApp->OnShutdown();

		m_userApp.reset();
		ShutdownImGui();

		RYU_LOG_INFO(LogEditorApp, "Editor application shutdown");
	}

	void EditorApp::OnTick(const Utils::TimeInfo& timeInfo)
	{
		RYU_PROFILE_SCOPE();
		m_userApp->OnTick(timeInfo);
	}

	bool EditorApp::RouteWndProc() const
	{
		RYU_PROFILE_SCOPE();
		s_originalWndProc = (WNDPROC)::SetWindowLongPtr(GetWindow()->GetHandle(), GWLP_WNDPROC, (LONG_PTR)&EditorApp::EditorWndProc);
		
		const bool success = s_originalWndProc != nullptr;
		if (success)
		{
			RYU_LOG_TRACE(LogEditorApp, "WndProc routed to EditorApp");
		}

		return success;
	}

	void EditorApp::InitImGui()
	{
		RYU_PROFILE_SCOPE();
		//IMGUI_CHECKVERSION();
		//ImGui::CreateContext();
		//ImGuiIO& io = ImGui::GetIO();
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
		//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // IF using Docking Branch

		//ImGui_ImplWin32_Init(GetWindow()->GetHWND());

		//bool result{ true };
		//switch (Graphics::GraphicsConfig::Get().GraphicsAPI)
		//{
		//	using namespace Graphics;
		//case Graphics::API::DirectX11:
		//	//result = ImGui_ImplDX11_Init(DX11::Core::GetDevice(), DX11::Core::GetImContext());
		//	break;

		//case Graphics::API::DirectX12:
		//	// result = ImGui_ImplDX12_Init(DX12::Core::GetDevice(), DX12::Core::GetDefaultRenderTargetFormat(), DX12::Core::GetSRVDescHeap(), )
		//	break;
		//}

		
		RYU_LOG_TRACE(LogEditorApp, "ImGui initialized");
	}

	void EditorApp::ShutdownImGui() const
	{
		//switch (Graphics::GraphicsConfig::Get().GraphicsAPI)
		//{
		//case Graphics::API::DirectX11:
		//	//ImGui_ImplDX11_Shutdown();
		//	break;

		//case Graphics::API::DirectX12:
		//	//ImGui_ImplDX12_Shutdown();
		//	break;
		//}
		
		//ImGui_ImplWin32_Shutdown();
		//ImGui::DestroyContext();
		RYU_LOG_INFO(LogEditorApp, "ImGui shutdown");
	}

	bool EditorApp::LoadGameModule()
	{
		RYU_PROFILE_SCOPE();

		std::unique_ptr<Game::IGameModule> gm(Game::CreateGameModule());
		if (gm)
		{
			m_userApp = gm->CreateApplication(GetWindow());
			GetWindow()->Title = std::format("Ryu Editor - {}", gm->GetName());
			return m_userApp != nullptr;
		}

		return false;
	}

	LRESULT EditorApp::EditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Call the ImGui window proc, then call our window proc
		std::ignore = ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return ::CallWindowProc(s_originalWndProc, hWnd, msg, wParam, lParam);
	}
}

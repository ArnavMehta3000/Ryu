#include "EditorApp.h"
#include "Game/IGameModule.h"
#include "Logging/Logger.h"
#include "Profiling/Profiling.h"
#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Ryu::Editor
{
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

		RYU_LOG_TRACE("Initializing editor application");

		if (!RouteWndProc())
		{
			RYU_LOG_ERROR("Failed to route Editor WndProc!");
			return false;
		}


		if (!LoadGameModule())
		{
			RYU_LOG_ERROR("Failed to load game module!");
			return false;
		}

		//Gfx::Renderer* renderer = Engine::Engine::Get().GetRenderer();
		//renderer->ImGuiCallback = [this](Gfx::Renderer* renderer) { OnImGui(renderer); };

		// Init user application
		RYU_LOG_TRACE("Initializing user application");
		m_userApp->m_isRunning = m_userApp->OnInit();
		return m_userApp->m_isRunning;
	}

	void EditorApp::OnShutdown()
	{
		RYU_PROFILE_SCOPE();

		RYU_LOG_TRACE("Shutting down editor application");

		m_userApp->OnShutdown();

		m_userApp.reset();

		RYU_LOG_INFO("Editor application shutdown");
	}

	void EditorApp::OnTick(const Utils::TimeInfo& timeInfo)
	{
		RYU_PROFILE_SCOPE();

		// Display FPS stats in debug mode
#if defined(RYU_BUILD_DEBUG)
		GetWindow()->Title = fmt::format("Ryu Editor | DT: {:.5f}ms | FPS: {}", timeInfo.DeltaTime, timeInfo.FPS);
#endif

		m_userApp->OnTick(timeInfo);
	}

	//void EditorApp::OnImGui([[maybe_unused]] Gfx::Renderer* renderer)
	//{
	//	ImGui::ShowDemoWindow();
	//}

	bool EditorApp::RouteWndProc() const
	{
		RYU_PROFILE_SCOPE();
		s_originalWndProc = (WNDPROC)::SetWindowLongPtr(GetWindow()->GetHandle(), GWLP_WNDPROC, (LONG_PTR)&EditorApp::EditorWndProc);

		const bool success = s_originalWndProc != nullptr;
		if (success)
		{
			RYU_LOG_TRACE("WndProc routed to EditorApp");
		}

		return success;
	}

	bool EditorApp::LoadGameModule()
	{
		RYU_PROFILE_SCOPE();

		std::unique_ptr<Game::IGameModule> gm(Game::CreateGameModule());
		if (gm)
		{
			m_userApp = gm->CreateApplication(GetWindow());
			GetWindow()->Title = fmt::format("Ryu Editor - {}", gm->GetName());
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

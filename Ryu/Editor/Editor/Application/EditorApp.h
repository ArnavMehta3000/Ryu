#pragma once
#include "App/Application.h"
#include "Graphics/Renderer.h"

namespace Ryu::Editor
{
	// Abstraction around the user application loaded via the game module
	class EditorApp : public App::App
	{
	public:
		EditorApp(std::shared_ptr<Window::Window> window);

	private:
		static LRESULT CALLBACK EditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		bool OnInit() override final;
		void OnShutdown() override final;
		void OnTick(const Utils::TimeInfo& timeInfo);

		void OnImGui(Gfx::Renderer* renderer);
		
		bool RouteWndProc() const;

		bool LoadGameModule();

	private:
		std::shared_ptr<App> m_userApp;
		Gfx::DescriptorHeap m_imguiHeap;
		Gfx::DescriptorHandle m_imguiHeapHandle;
	};
}

#pragma once
#include "Graphics/IRendererHook.h"
#include "Graphics/Core/GfxDescriptorHeap.h"
#include "Application/App/Application.h"

namespace Ryu::Editor
{
	// Abstraction around the user application loaded via the game module
	class EditorApp 
		: public App::App
		, public Gfx::IRendererHook
	{
	public:
		EditorApp(std::shared_ptr<Window::Window> window);

		[[nodiscard]] inline Game::WorldManager* GetWorldManager() noexcept { return m_userApp ? m_userApp->GetWorldManager() : nullptr; }

	private:
		bool OnInit() override final;
		void OnShutdown() override final;
		void OnTick(const Utils::FrameTimer& timeInfo);

		bool LoadGameModule();

#if defined(RYU_WITH_EDITOR)
		void OnImGuiSetup(Gfx::Device* device) override;
		void OnImGuiFrameBegin() override;
		void OnImGuiFrameEnd(Gfx::CommandList* cmdList) override;
		void OnImGuiRender() override;
		void OnImGuiShutdown() override;
#endif
	private:
		Game::WorldManager*                  m_worldManager;
		std::shared_ptr<App>                 m_userApp;
		std::unique_ptr<Gfx::DescriptorHeap> m_imguiHeap;
	};
}

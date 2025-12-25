#pragma once
#include "Core/Utils/Timing/FrameTimer.h"
#include "Application/Window/Window.h"
#include "Core/Logging/Logger.h"

namespace Ryu::Engine { class Engine; }
namespace Ryu::Editor { class EditorApp; }
namespace Ryu::Game   { class World; class WorldManager; }

namespace Ryu::App
{
	class App
	{
		friend class Engine::Engine;
		friend class Editor::EditorApp;
	public:
		static void InitWindow(Window::Window& window);

		explicit App(std::shared_ptr<Window::Window> window);  // Use existing window
		explicit App(const Window::Window::Config& config);  // Let the application create the window
		virtual ~App();

		void Quit();

		[[nodiscard]] inline std::shared_ptr<Window::Window> GetWindow() const noexcept { return m_window; }
		[[nodiscard]] inline bool IsRunning() const noexcept { return m_isRunning; }
		[[nodiscard]] inline virtual Game::WorldManager* GetWorldManager() noexcept { return nullptr; }

	protected:
		virtual bool OnInit() = 0;
		virtual void OnTick(const Utils::FrameTimer&) = 0;
		virtual void OnShutdown() = 0;

	private:
		void ProcessWindowEvents();

	private:
		bool                            m_isRunning = false;
		std::shared_ptr<Window::Window> m_window;
	};
}

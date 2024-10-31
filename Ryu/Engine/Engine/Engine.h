#pragma once
#include "Config/CommandLine.h"
#include "Logger/Logger.h"
#include "Graphics/ISurface.h"
#include "Event/Event.h"
#include <memory>

namespace Ryu::App { class Application; }

namespace Ryu::Engine
{
	/**
	 * @brief The main Engine class
	 */
	class Engine
	{
		RYU_DECLARE_SINGLETON(Engine, true);
		RYU_LOG_CATEGORY(Engine);

	public:
		Engine();
		~Engine();

		/**
		 * @brief Set the application that is run by the Engine
		 * @param app The application to use (owned by the Engine after setting)
		 */
		void SetApp(std::shared_ptr<App::Application> app) { m_app = std::move(app); }

		/**
		 * @brief Set the command line used by the Engine
		 * @param cmdLine Application command line
		 */
		void SetCommandLine(std::wstring_view cmdLine);
		

		/**
		 * @brief Get the application used by the Engine
		 * @return The application (owned by the Engine)
		 */
		inline std::shared_ptr<App::Application> GetApp() const { return m_app; }

		/**
		 * @brief Get the command line used by the Engine
		 * @return The parsed command line
		 */
		inline const Config::CommandLine& GetCommdandLine() const { return m_cmdLine; }

		/**
		 * @brief Get the Engine up time in seconds
		 * @return How long the engine has been active for
		 */
		static f64 GetEngineUpTime();

		/**
		 * @brief Run the main engine loop
		 */
		void Run();

		/**
		 * @brief Stop the engine
		 * @note This reqests the application to stop running
		 */
		void Quit() const noexcept;

	private:
		bool Init();
		bool InitApplication();
		void Shutdown();
		void DoFrame(f64 dt) const;
		void OnAppResize(u32 width, u32 height) const noexcept;

	private:
		std::shared_ptr<App::Application> m_app;
		Config::CommandLine               m_cmdLine;
		Graphics::GFXSurface              m_renderSurface;
	};
}

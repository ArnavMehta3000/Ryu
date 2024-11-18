#pragma once
#include "Config/CommandLine.h"
#include "Logger/Logger.h"
#include "Graphics/ISurface.h"
#include "Event/Event.h"
#include <memory>

namespace Ryu::Engine
{
	class Runtime;

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
		void SetRuntime(std::shared_ptr<Runtime> runtime) { m_runtime = std::move(runtime); }

		/**
		 * @brief Set the command line used by the Engine
		 * @param cmdLine Application command line
		 */
		void SetCommandLine(std::wstring_view cmdLine);
		

		/**
		 * @brief Get the application used by the Engine
		 * @return The application (owned by the Engine)
		 */
		inline RYU_API std::shared_ptr<Runtime> GetRuntime() const { return m_runtime; }

		/**
		 * @brief Get the command line used by the Engine
		 * @return The parsed command line
		 */
		inline RYU_API const Config::CommandLine& GetCommdandLine() const { return m_cmdLine; }

		/**
		 * @brief Get the Engine up time in seconds
		 * @return How long the engine has been active for
		 */
		static RYU_API f64 GetEngineUpTime();

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
		bool InitRuntime();
		void Shutdown();
		void DoFrame(f64 dt);
		void OnAppResize(u32 width, u32 height) const noexcept;

	private:
		std::shared_ptr<Runtime> m_runtime;
		Config::CommandLine      m_cmdLine;
		Graphics::GFXSurface     m_renderSurface;
	};

	std::string_view GetEngineDirectory();
}

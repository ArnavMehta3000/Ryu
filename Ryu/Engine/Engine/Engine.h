#pragma once
#include "App/Application.h"
#include "Config/CommandLine.h"
#include "Graphics/Renderer.h"
#include "Logger/LogCategory.h"
#include "Utils/Singleton.h"
#include <memory>

namespace Ryu::Engine
{
	/**
	 * @brief The main Engine class
	 */
	class Engine : public Utils::Singleton<Engine>
	{
		RYU_LOG_CATEGORY(Engine);
		RYU_DECLARE_SINGLETON(Engine);

	public:
		~Engine() = default;

		/**
		 * @brief Set the command line used by the Engine
		 * @param cmdLine Application command line
		 */
		void SetCommandLine(std::wstring_view cmdLine);
		

		/**
		 * @brief Get the application used by the Engine
		 * @return The application (owned by the Engine)
		 */
		inline RYU_API App::Application* GetApplication() const { return m_app.get(); }

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

	protected:
		Engine();

	private:
		bool Init();
		bool InitRuntime();
		void Shutdown();
		void DoFrame(f64 dt);
		void OnAppResize(u32 width, u32 height) const noexcept;

	private:
		Config::CommandLine                 m_cmdLine;
		std::shared_ptr<App::Application>   m_app;
		std::unique_ptr<Graphics::Renderer> m_renderer;
	};
}

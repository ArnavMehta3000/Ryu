#pragma once
#include "App/Application.h"
#include "Graphics/Renderer.h"
#include "Logger/LogCategory.h"
#include "Utils/Singleton.h"
#include "Scripting/ScriptEngine.h"
#include "Engine/Game/GameModuleLoader.h"
#include <memory>

namespace Ryu::Engine
{
	/**
	 * @brief The main Engine class
	 */
	class Engine : public Utils::Singleton<Engine>
	{
		RYU_LOG_DECLARE_CATEGORY(Engine);
		RYU_SINGLETON_DECLARE(Engine);

	public:
		~Engine() = default;

		/**
		 * @brief Get the application used by the Engine
		 * @return The application (owned by the Engine)
		 */
		inline RYU_API App::Application* GetApplication() const { return m_app.get(); }

		/**
		 * @brief Get the timer used by the Engine
		 * @return The timer
		 */
		inline RYU_API const Utils::Timer& GetTimer() const { return m_timer; }

		/**
		 * @brief Get the Engine up time in seconds
		 * @return How long the engine has been active for
		 */
		RYU_API f64 GetEngineUpTime();

		/**
		 * @brief Run the main engine loop
		 */
		void Run();

		/**
		 * @brief Stop the engine
		 * @note This reqests the application to stop running
		 */
		void Quit() const noexcept;

		void RYU_API RunWithGameModule(const std::string& gameDllPath);
		bool RYU_API IsGameModuleLoaded() const;
		inline NODISCARD GameModuleLoader* GetGameModuleLoader() const { return m_gameModuleLoader.get(); }

	protected:
		Engine();

	private:
		bool Init();
		bool InitRuntime();
		void Shutdown();
		bool LoadGameModule(const std::string& gameDllPath);
		void UnloadGameModule();
		void DoFrame(const Utils::TimeInfo& timeInfo);
		void OnAppResize(u32 width, u32 height) const noexcept;

	private:
		Utils::Timer                                  m_timer;
		std::unique_ptr<App::Application>             m_app;
		std::unique_ptr<Gfx::Renderer>                m_renderer;
		std::unique_ptr<Scripting::ScriptEngine>      m_scriptEngine;
		std::unique_ptr<GameModuleLoader>             m_gameModuleLoader;
		Elos::Connection<const Elos::Event::Resized&> m_onAppResizedConnection;
	};
}

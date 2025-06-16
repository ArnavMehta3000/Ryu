#pragma once
#include "App/Application.h"
#include "Graphics/Renderer.h"
#include "Logger/LogCategory.h"
#include "Utils/Singleton.h"
#include "Scripting/ScriptEngine.h"
#include "Engine/Game/GameModuleLoader.h"
#include <memory>
#include <Elos/Window/Window.h>

namespace Ryu::Engine
{
	class Engine : public Utils::Singleton<Engine>
	{
		RYU_LOG_DECLARE_CATEGORY(Engine);
		RYU_SINGLETON_DECLARE(Engine);

	public:
		~Engine() = default;
		inline RYU_API std::shared_ptr<App::App> GetApplication() const { return m_app; }
		inline RYU_API const Utils::Timer& GetTimer() const { return m_timer; }
		RYU_API f64 GetEngineUpTime();
		void Quit() const noexcept;

		void RYU_API RunApp(std::shared_ptr<App::App> app);
		void RYU_API RunWithGameModule(const std::string& gameDllPath);
		bool RYU_API IsGameModuleLoaded() const;
		inline NODISCARD GameModuleLoader* GetGameModuleLoader() const { return m_gameModuleLoader.get(); }

	protected:
		Engine();

	private:
		bool Init();
		void Shutdown();
		bool LoadGameModule(const std::string& gameDllPath);
		void UnloadGameModule();
		void OnAppResize(u32 width, u32 height) const noexcept;

	private:
		Utils::Timer                                  m_timer;
		std::shared_ptr<App::App>             m_app;
		std::unique_ptr<Gfx::Renderer>                m_renderer;
		std::unique_ptr<Scripting::ScriptEngine>      m_scriptEngine;
		std::unique_ptr<GameModuleLoader>             m_gameModuleLoader;
	};
}

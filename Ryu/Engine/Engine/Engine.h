#pragma once
#include "App/Application.h"
#include "Engine/Plugin/EnginePlugin.h"
#include "Utils/Singleton.h"
#include "Utils/ServiceLocator.h"
#include "Graphics/Renderer.h"
#include "Logger/LogCategory.h"
#include "Scripting/ScriptEngine.h"
#include <memory>
#include <stacktrace>

namespace Ryu::Engine
{
	static void PrintMemoryStats();

	class Engine : public Utils::Singleton<Engine>
	{
		RYU_SINGLETON_DECLARE(Engine);

	public:
		~Engine() = default;
		inline RYU_API std::shared_ptr<App::App> GetApplication() const { return m_app; }
		inline RYU_API Gfx::Renderer* GetRenderer() { return m_renderer.get(); }
		inline RYU_API const Utils::Timer& GetTimer() const { return m_timer; }
		RYU_API f64 GetEngineUpTime();
		RYU_API void Quit() const noexcept;

		void RYU_API RunApp(std::shared_ptr<App::App> app);

	protected:
		Engine() {}

	private:
		bool Init();
		void Shutdown();
		void MainLoop();
		void OnAppResize(u32 width, u32 height) const noexcept;
		void SetupLogger();
		void InitializePlugins(Plugin::PluginPhase phase);
		void ShutdownPlugins(Plugin::PluginPhase phase);

	private:
		Utils::Timer                                  m_timer;
		Plugin::PluginManager                         m_pluginManager;
		EngineContext                                 m_engineContext;
		std::shared_ptr<App::App>                     m_app;
		std::unique_ptr<Gfx::Renderer>                m_renderer;
		std::unique_ptr<Scripting::ScriptEngine>      m_scriptEngine;
	};

	Utils::ServiceLocator& GetServiceLocator();
}

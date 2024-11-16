#pragma once
#include "App/Application.h"
#include "Logger/LogCategory.h"
#include "Scripting/ScriptEngine.h"
#include "World/World.h"

namespace Ryu::Engine
{
	class Runtime : public App::Application
	{
		RYU_LOG_CATEGORY(Runtime);

	public:
		Runtime();
		inline constexpr World::World& GetWorld() noexcept { return m_world; }
		inline constexpr Scripting::ScriptEngine& GetScriptEngine() noexcept { return m_scriptEngine; }

	protected:
		virtual bool OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnTick(f64 dt) override;
		virtual void OnRegisterWorldSubsystems();

	private:
		void ConfigureScriptEngine();
		void RegisterEngineWithScriptEngine();

	private:
		World::World            m_world;
		Scripting::ScriptEngine m_scriptEngine;
	};
}

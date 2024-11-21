#include "Runtime.h"
#include "Logger/Logger.h"
#include "Engine/Registration/ScriptRegistration.h"

// Example subsystem
class ExampleSubsystem : public Ryu::Game::IWorldSubsystem
{
public:
	ExampleSubsystem(Ryu::Game::World* world) : IWorldSubsystem(world) { }

private:
	// Inherited via IWorldSubsystem
	std::string_view GetName() override { return "ExampleSubsystem"; }
	bool OnInit() override { return true; }	
	void OnShutdown() override {}	
	void OnTick(MAYBE_UNUSED f64 dt) override {}
};

namespace
{
	void DebugLog(const std::string& str)
	{
		RYU_LOG_CATEGORY(Script);
		LOG_INFO(RYU_USE_LOG_CATEGORY(Script), "{}", str);
	}
}


namespace Ryu::Engine
{
	Runtime::Runtime()
		: m_world()
	{
		LOG_TRACE(RYU_USE_LOG_CATEGORY(Runtime), "Runtime created");
	}

	bool Runtime::OnInit()
	{
		LOG_TRACE(RYU_USE_LOG_CATEGORY(Runtime), "Initializing Runtime");

		if (!m_scriptEngine.Init())
		{
			LOG_FATAL(RYU_USE_LOG_CATEGORY(Runtime), "Failed to initialize script engine");
			return false;
		}

		ConfigureScriptEngine();

		// First register world subsystems, then init the world
		OnRegisterWorldSubsystems();
		m_world.Init();

		LOG_DEBUG(RYU_USE_LOG_CATEGORY(Runtime), "Runtime initialized");
		return true;
	}
	
	void Runtime::OnShutdown()
	{
		LOG_TRACE(RYU_USE_LOG_CATEGORY(Runtime), "Shutting down Runtime");

		m_scriptEngine.Shutdown();
		m_world.Shutdown();
		
		LOG_DEBUG(RYU_USE_LOG_CATEGORY(Runtime), "Runtime shut down");
	}
	
	void Runtime::OnTick(f64 dt)
	{
		m_world.Tick(dt);
		//if (m_gameInstance)
		//{
		//	m_gameInstance->OnUpdate(dt);
		//}
	}

	void Runtime::OnRegisterWorldSubsystems()
	{
		GetWorld().RegisterSubsystem<ExampleSubsystem>();
	}
	
	void Runtime::ConfigureScriptEngine()
	{
		LOG_DEBUG(RYU_USE_LOG_CATEGORY(Runtime), "Configuring script engine for runtime use");
		
		ScriptRegistration::Register(m_scriptEngine.GetEngine());
	}
}

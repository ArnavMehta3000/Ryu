#include "Runtime.h"
#include "Logger/Logger.h"
#include "Engine/Engine.h"
#include <angelscript.h>
#include <External/AngelScript/AddOns/autowrapper/aswrappedcall.h>

namespace
{
	Ryu::Engine::Engine* GetEngineWrapper()
	{
		return Ryu::Engine::Engine::Get();
	}

	f64 GetEngineUpTimeWrapper()
	{
		return Ryu::Engine::Engine::GetEngineUpTime();
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

		// World initialization is handled by the engine
		// Since we want to initialize the world after the 
		// application has been initialized.
		// This is because the world is a member of the runtime

		if (!m_scriptEngine.Init())
		{
			LOG_FATAL(RYU_USE_LOG_CATEGORY(Runtime), "Failed to initialize script engine");
			return false;
		}

		ConfigureScriptEngine();

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
	}
	
	void Runtime::ConfigureScriptEngine()
	{
		LOG_DEBUG(RYU_USE_LOG_CATEGORY(Runtime), "Configuring script engine for runtime use");
		RegisterEngineWithScriptEngine();
	}
	
	void Runtime::RegisterEngineWithScriptEngine()
	{
		i32 r{ 0 };
		auto* const scriptEngine = m_scriptEngine.GetEngine();
		
		// Register Engine type
		r = scriptEngine->RegisterObjectType("Engine", sizeof(Ryu::Engine::Engine), asOBJ_REF | asOBJ_NOCOUNT);
		ASSERT(r >= 0);

		// Register Engine up time getter
		r = scriptEngine->RegisterGlobalFunction("double GetEngineUpTime()", WRAP_FN(GetEngineUpTimeWrapper), asCALL_GENERIC);
		ASSERT(r >= 0);

		// Register Engine getter
		r = scriptEngine->RegisterGlobalFunction("Engine@ GetEngine()", WRAP_FN(GetEngineWrapper), asCALL_GENERIC);
		ASSERT(r >= 0);
	}
}


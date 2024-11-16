#include "Runtime.h"
#include "Logger/Logger.h"
#include "Engine/Engine.h"
#include <angelscript.h>
#include <External/AngelScript/AddOns/scriptbuilder/scriptbuilder.h>

// Example subsystem
class ExampleSubsystem : public Ryu::Game::IWorldSubsystem
{
public:
	ExampleSubsystem(Ryu::Game::World* world) : IWorldSubsystem(world)
	{

	}

private:
	// Inherited via IWorldSubsystem
	std::string_view GetName() override { return "ExampleSubsystem"; }
	bool OnInit() override { return true; }	
	void OnShutdown() override {}	
	void OnTick(MAYBE_UNUSED f64 dt) override {}
};

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

		// First register world subsystems, then init the world
		OnRegisterWorldSubsystems();
		m_world.Init();

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

	void Runtime::OnRegisterWorldSubsystems()
	{
		GetWorld().RegisterSubsystem<ExampleSubsystem>();
	}
	
	void Runtime::ConfigureScriptEngine()
	{
		LOG_DEBUG(RYU_USE_LOG_CATEGORY(Runtime), "Configuring script engine for runtime use");
		RegisterEngineAPI();
	}
	
	void Runtime::RegisterEngineAPI()
	{
		LOG_TRACE(RYU_USE_LOG_CATEGORY(Runtime), "Registering Engine API");
		i32 r{ 0 };
		auto* const scriptEngine = m_scriptEngine.GetEngine();
		
		// Register Engine type
		r = scriptEngine->RegisterObjectType("Engine", 0, asOBJ_REF | asOBJ_NOCOUNT);
		ASSERT(r >= 0);

		// Register Engine up time getter
		r = scriptEngine->RegisterGlobalFunction("double GetEngineUpTime()", asFUNCTION(GetEngineUpTimeWrapper), asCALL_CDECL);
		ASSERT(r >= 0);

		// Register Engine getter
		r = scriptEngine->RegisterGlobalFunction("Engine@ GetEngine()", asFUNCTION(GetEngineWrapper), asCALL_CDECL);
		ASSERT(r >= 0);

		// Register engine quit function
		r = scriptEngine->RegisterObjectMethod("Engine", "void Quit() const", asMETHOD(Ryu::Engine::Engine, Quit), asCALL_THISCALL);
		ASSERT(r >= 0);

		r = scriptEngine->RegisterGlobalFunction("void DebugLog(const string& in)", asFUNCTION(DebugLog), asCALL_CDECL);
		ASSERT(r >= 0);

		// Sample script as a string
		const char* testScript = R"(
		[editable]
		[range [10, 100]]
		void main()
		{
			DebugLog("This is an example message from a custom script!");
		})";

		CScriptBuilder builder;
		if (builder.StartNewModule(scriptEngine, "TestModule") >= 0)
		{
			if (builder.AddSectionFromMemory("TestScript", testScript) >= 0)
			{
				if (builder.BuildModule() >= 0)
				{
					if (asIScriptModule* mod = scriptEngine->GetModule("TestModule"))
					{
						if (asIScriptFunction* func = mod->GetFunctionByDecl("void main()"))
						{
							std::vector<std::string> metadata = builder.GetMetadataForFunc(func);

							if (asIScriptContext* ctx = scriptEngine->CreateContext())
							{
								if (ctx->Prepare(func) >= 0)
								{
									r = ctx->Execute();
									if (r != asEXECUTION_FINISHED)
									{
										// The execution didn't complete as expected. Determine what happened.
										if (r == asEXECUTION_EXCEPTION)
										{
											LOG_ERROR(RYU_USE_LOG_CATEGORY(Runtime), "Failed to execute script: {}", ctx->GetExceptionString());
										}
									}
								}

								ctx->Release();
							}
						}
					}
				}
			}
		}
	}
}


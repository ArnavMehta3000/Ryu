#include "ScriptRegistration.h"
#include "Engine/Engine.h"
#include "Logger/Logger.h"
#include <angelscript.h>

#define REGISTER_CHECK(X) r = X; ASSERT(r >= 0)
#define DEFINE_API(X) void RYU_CONCAT(ScriptRegistration::Register, RYU_CONCAT(X, API))(asIScriptEngine* engine)
#define CALL_API(X) RYU_CONCAT(Register, RYU_CONCAT(X, API))(engine)


namespace  // Anonymous namespace for proxy functions to get register
{
	i32 r = 0;

	void RyuLog(Ryu::Logging::LogLevel level, const std::string& message)
	{
		RYU_LOG_CATEGORY(Script);

		switch (level)
		{
			using enum Ryu::Logging::LogLevel;

		case Trace: 
			LOG_TRACE(RYU_USE_LOG_CATEGORY(Script), "{}", message);
			return;
		
		case Debug: 
			LOG_DEBUG(RYU_USE_LOG_CATEGORY(Script), "{}", message);
			return;
		
		case Info: 
			LOG_INFO(RYU_USE_LOG_CATEGORY(Script), "{}", message);
			return;
		
		case Warn: 
			LOG_WARN(RYU_USE_LOG_CATEGORY(Script), "{}", message);
			return;
		
		default:
			LOG_INFO(RYU_USE_LOG_CATEGORY(Script), "<LogLevel {} Not Supported> {}", Ryu::EnumToString(level), message);
			return;
		}
	}
}


namespace Ryu::Engine
{	
	void ScriptRegistration::Register(asIScriptEngine* engine)
	{
		CALL_API(Logger);
		CALL_API(Engine);
		CALL_API(Debug);
		CALL_API(World);
		CALL_API(GameInstance);
	}

	DEFINE_API(Logger)
	{
		REGISTER_CHECK(engine->RegisterEnum("LogLevel"));
		REGISTER_CHECK(engine->RegisterEnumValue("LogLevel", "Trace", (i32)Ryu::Logging::LogLevel::Trace));
		REGISTER_CHECK(engine->RegisterEnumValue("LogLevel", "Debug", (i32)Ryu::Logging::LogLevel::Debug));
		REGISTER_CHECK(engine->RegisterEnumValue("LogLevel", "Info", (i32)Ryu::Logging::LogLevel::Info));
		REGISTER_CHECK(engine->RegisterEnumValue("LogLevel", "Warn", (i32)Ryu::Logging::LogLevel::Warn));
	}

	DEFINE_API(Engine)
	{
		REGISTER_CHECK(engine->RegisterObjectType("Engine", 0, asOBJ_REF | asOBJ_NOCOUNT));
		REGISTER_CHECK(engine->RegisterGlobalFunction("Engine@ GetEngine()", asFUNCTION(::Ryu::Engine::Engine::Get), asCALL_CDECL));
		REGISTER_CHECK(engine->RegisterObjectMethod("Engine", "void Quit() const", asMETHOD(Ryu::Engine::Engine, Quit), asCALL_THISCALL));
	}

	DEFINE_API(Debug)
	{
		REGISTER_CHECK(engine->RegisterGlobalFunction("void RyuLog(LogLevel, string)", asFUNCTION(RyuLog), asCALL_CDECL));
	}

	DEFINE_API(World)
	{
		
	}

	DEFINE_API(GameInstance)
	{
		REGISTER_CHECK(engine->RegisterInterface("IGameInstance"));
		REGISTER_CHECK(engine->RegisterInterfaceMethod("IGameInstance", "void OnStart()"));
		REGISTER_CHECK(engine->RegisterInterfaceMethod("IGameInstance", "void OnEnd()"));
		REGISTER_CHECK(engine->RegisterInterfaceMethod("IGameInstance", "void OnUpdate(double)"));
	}
}

#undef CALL_API
#undef DEFINE_API
#undef REGISTER_CHECK

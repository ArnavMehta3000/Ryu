#include "GameInstanceFactory.h"
#include "ScriptGameInstanceWrapper.h"
#include "Game/GameInstance/ScriptableGameInstance.h"
#include "Logger/Logger.h"
#include <angelscript.h>

namespace Ryu::Engine
{
	GameInstanceFactory::GameInstanceFactory(asIScriptEngine* engine)
		: m_scriptEngine(engine)
	{
	}
	
	std::unique_ptr<Game::ScriptableGameInstance> GameInstanceFactory::CreateInstance(const std::string& className)
	{
		// Get script class type
		asITypeInfo* type = m_scriptEngine->GetTypeInfoByName(className.c_str());
		if (!type)
		{
			LOG_ERROR(RYU_USE_LOG_CATEGORY(GameInstanceFactory), "Failed to get type info for class: {}", className);
			return nullptr;
		}

		// Get factory function
		asIScriptFunction* factoryFunc = type->GetFactoryByDecl((className + " @" + className + "()").c_str());
		if (!factoryFunc)
		{
			LOG_ERROR(RYU_USE_LOG_CATEGORY(GameInstanceFactory), "Failed to get factory function for class: {}", className);
			return nullptr;
		}

		// Create context for factory execution
		asIScriptContext* context = m_scriptEngine->CreateContext();
		context->Prepare(factoryFunc);

		// Execute the factory function
		i32 r = context->Execute();
		if (r != asEXECUTION_FINISHED)
		{
			if (r == asEXECUTION_EXCEPTION)
			{
				LOG_ERROR(RYU_USE_LOG_CATEGORY(GameInstanceFactory), "Failed to execute factory function for class: {} - {}", className, context->GetExceptionString());
			}
			context->Release();
			return nullptr;
		}

		// Get instance
		asIScriptObject* obj = *static_cast<asIScriptObject**>(context->GetAddressOfReturnValue());
		obj->AddRef();
		context->Release();

		return std::make_unique<ScriptGameInstanceWrapper>(obj);
	}
}

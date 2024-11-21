#include "ScriptableGameInstance.h"
#include "Logger/Logger.h"
#include "Scripting/Helpers.h"
#include <libassert/assert.hpp>

namespace Ryu::Game
{
	ScriptableGameInstance::ScriptableGameInstance()
		: m_onStartFunc(nullptr)
		, m_onEndFunc(nullptr)
		, m_onUpdateFunc(nullptr)
		, m_factoryFunc(nullptr)
		, m_object(nullptr)
	{
	}

	void ScriptableGameInstance::CacheData(CScriptBuilder& builder)
	{
		// Call base class
		IScriptableObject::CacheData(builder);

		asITypeInfo* type{ nullptr };
		const i32 typeCount = m_module->GetObjectTypeCount();
		
		bool found{ false };
		for (i32 i = 0; i < typeCount; i++)
		{
			if (type = m_module->GetObjectTypeByIndex(i); type)
			{
				i32 interfaceCount = type->GetInterfaceCount();
				if (interfaceCount == 0 || interfaceCount > 1)
				{
					LOG_ERROR(RYU_USE_LOG_CATEGORY(ScriptableGameInstance), 
						"IGameInstance must be implemented only once! Implementations found: {}", interfaceCount);
					LOG_ERROR(RYU_USE_LOG_CATEGORY(ScriptableGameInstance), "Using Type: {}", type->GetName());
					break;
				}

				// Only consider the first interface
				std::string interfaceName = type->GetInterface(0)->GetName();
				if (interfaceName == "IGameInstance")
				{
					found = true;
					break;
				}
			}
		}

		if (found && type)
		{
			// Check if user provided a friendly name
			const std::vector<std::string> metadata = builder.GetMetadataForType(type->GetTypeId());			
			m_friendlyName = Scripting::GetFriendlyNameFromMetadata(metadata).value_or(type->GetName());
			
			m_onStartFunc  = type->GetMethodByDecl("void OnStart()");
			m_onEndFunc    = type->GetMethodByDecl("void OnEnd()");
			m_onUpdateFunc = type->GetMethodByDecl("void OnUpdate(double)");

			// Get factory function
			auto resultName = Scripting::ExtractFactoryFunctionName(metadata);
			if (resultName.has_value())
			{
				const std::string name = resultName.value();
				
				std::string factoryFuncName = type->GetName();
				factoryFuncName += " @";
				factoryFuncName += name;
				factoryFuncName += "()";

				m_factoryFunc = type->GetFactoryByDecl(factoryFuncName.c_str());
				LOG_INFO(RYU_USE_LOG_CATEGORY(ScriptableGameInstance), "Found factory function: {}", factoryFuncName);
				LOG_INFO(RYU_USE_LOG_CATEGORY(ScriptableGameInstance), "Using friendly name: {}", m_friendlyName);
			}
			else
			{
				LOG_ERROR(RYU_USE_LOG_CATEGORY(ScriptableGameInstance), "Failed to get factory function name for type: {}", m_friendlyName);
			}
		}
	}

	void ScriptableGameInstance::CreateObject(asIScriptContext* ctx)
	{
		if (!m_factoryFunc)
		{
			LOG_ERROR(RYU_USE_LOG_CATEGORY(ScriptableGameInstance), "Game instance has no factory function");
			return;
		}

		ctx->Prepare(m_factoryFunc);
		ctx->Execute();
		m_object = *(asIScriptObject**)ctx->GetAddressOfReturnValue();
		m_object->AddRef();
	}
	
	void ScriptableGameInstance::CallOnStart(asIScriptContext* ctx)
	{
		if (!m_onStartFunc || !m_object)
		{
			LOG_WARN(RYU_USE_LOG_CATEGORY(ScriptableGameInstance), "IGameInstance::OnStart() is not implemented!");
			return;
		}
		
		ctx->Prepare(m_onStartFunc);
		ctx->SetObject(m_object);
		ctx->Execute();
	}
	
	void ScriptableGameInstance::CallOnEnd(asIScriptContext* ctx)
	{
		if (!m_onEndFunc || !m_object)
		{
			LOG_WARN(RYU_USE_LOG_CATEGORY(ScriptableGameInstance), "IGameInstance::OnEnd() is not implemented!");
			return;
		}
		
		ctx->Prepare(m_onEndFunc);
		ctx->SetObject(m_object);
		ctx->Execute();

		m_object->Release();
	}
	
	void ScriptableGameInstance::CallOnUpdate(asIScriptContext* ctx, f64 dt)
	{
		if (!m_onUpdateFunc || !m_object)
		{
			LOG_WARN(RYU_USE_LOG_CATEGORY(ScriptableGameInstance), "IGameInstance::OnUpdate(double) is not implemented!");
			return;
		}
		
		ctx->Prepare(m_onUpdateFunc);
		ctx->SetObject(m_object);
		ctx->SetArgDouble(0, dt);
		ctx->Execute();
	}
}

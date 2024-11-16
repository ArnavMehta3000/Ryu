#include "ScriptGameInstanceWrapper.h"
#include <angelscript.h>

namespace Ryu::Engine
{
	ScriptGameInstanceWrapper::ScriptGameInstanceWrapper(asIScriptObject* obj)
		: m_scriptObj(obj)
	{
		asITypeInfo* type = obj->GetObjectType();
		
		m_initMethod     = type->GetMethodByName("OnInit");
		m_startMethod    = type->GetMethodByName("OnStart");
		m_stopMethod     = type->GetMethodByName("OnStop");
		m_updateMethod   = type->GetMethodByName("OnUpdate");
		m_shutdownMethod = type->GetMethodByName("OnShutdown");
	}
	
	ScriptGameInstanceWrapper::~ScriptGameInstanceWrapper()
	{
		if (m_scriptObj)
		{
			m_scriptObj->Release();
		}
	}
	
	void ScriptGameInstanceWrapper::OnInit()
	{
		ExecuteMethod(m_initMethod);
	}
	
	void ScriptGameInstanceWrapper::OnStart()
	{
		ExecuteMethod(m_startMethod);
	}
	
	void ScriptGameInstanceWrapper::OnStop()
	{
		ExecuteMethod(m_stopMethod);
	}
	
	void ScriptGameInstanceWrapper::OnUpdate(f64 deltaTime)
	{
		asIScriptContext* ctx = m_scriptObj->GetEngine()->CreateContext();
		ctx->Prepare(m_updateMethod);
		ctx->SetObject(m_scriptObj);
		ctx->SetArgDouble(0, deltaTime);
		ctx->Execute();
		ctx->Release();
	}
	
	void ScriptGameInstanceWrapper::OnShutdown()
	{
		ExecuteMethod(m_shutdownMethod);
	}
	
	void ScriptGameInstanceWrapper::ExecuteMethod(asIScriptFunction* method)
	{
		asIScriptContext* ctx = m_scriptObj->GetEngine()->CreateContext();
		ctx->Prepare(method);
		ctx->SetObject(m_scriptObj);
		ctx->Execute();
		ctx->Release();
	}
}

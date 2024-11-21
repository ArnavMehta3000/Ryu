#include "IScriptableObject.h"
#include "Scripting/ScriptEngine.h"

namespace Ryu::Scripting
{
	IScriptableObject::IScriptableObject()
		: m_refCount(1)
		, m_module(nullptr)
		, m_weakRefFlag(nullptr)
	{
	}
	
	IScriptableObject::~IScriptableObject()
	{
		if (m_weakRefFlag)
		{
			m_weakRefFlag->Set(true);
			m_weakRefFlag->Release();
		}
	}

	void IScriptableObject::CacheData(CScriptBuilder& builder)
	{
		m_module = builder.GetModule();
	}

	asILockableSharedBool* IScriptableObject::GetWeakRefFlag()
	{
		if (!m_weakRefFlag)
		{
			m_weakRefFlag = asCreateLockableSharedBool();
		}
		return m_weakRefFlag;
	}
}

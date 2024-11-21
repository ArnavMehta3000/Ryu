#pragma once
#include "Scripting/IScriptableObject.h"
#include "Logger/LogCategory.h"

namespace Ryu::Game
{
	class ScriptableGameInstance : public Scripting::IScriptableObject
	{
		RYU_LOG_CATEGORY(ScriptableGameInstance);

	public:
		ScriptableGameInstance();
		virtual ~ScriptableGameInstance() = default;

		void CacheData(CScriptBuilder& builder) override;
		void CreateObject(asIScriptContext* ctx);
		void CallOnStart(asIScriptContext* ctx);
		void CallOnEnd(asIScriptContext* ctx);
		void CallOnUpdate(asIScriptContext* ctx, f64 dt);

	private:
		asIScriptFunction* m_onStartFunc;
		asIScriptFunction* m_onEndFunc;
		asIScriptFunction* m_onUpdateFunc;
		asIScriptFunction* m_factoryFunc;
		asIScriptObject* m_object;
	};
}

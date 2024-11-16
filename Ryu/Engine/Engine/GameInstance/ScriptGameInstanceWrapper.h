#pragma once
#include "Game/GameInstance/ScriptableGameInstance.h"

class asIScriptObject;
class asIScriptFunction;

namespace Ryu::Engine
{
	class ScriptGameInstanceWrapper : public Game::ScriptableGameInstance
	{
	public:
		ScriptGameInstanceWrapper(asIScriptObject* obj);
		~ScriptGameInstanceWrapper();

		void OnInit() override;
		void OnStart() override;
		void OnStop() override;
		void OnUpdate(f64 deltaTime) override;
		void OnShutdown() override;

	private:
		void ExecuteMethod(asIScriptFunction* method);

	private:
		asIScriptObject* m_scriptObj;
		asIScriptFunction* m_initMethod;
		asIScriptFunction* m_startMethod;
		asIScriptFunction* m_stopMethod;
		asIScriptFunction* m_updateMethod;
		asIScriptFunction* m_shutdownMethod;
	};
}

#include "Test.h"
#include "Logger/Logger.h"
#include <External/AngelScript/AddOns/scriptbuilder/scriptbuilder.h>

bool TestApp::OnInit()
{
	if (!Runtime::OnInit())
		return false;

	/*const Ryu::Scripting::ScriptEngine& engine = GetScriptEngine();
	m_scriptContext = engine.CreateContext();

	CScriptBuilder builder;
	i32 r = builder.StartNewModule(engine.GetEngine(), "GameCode");
	if (r >= 0)
		r = builder.AddSectionFromFile("Game/Scripts/GameInstance.as");
	if (r >= 0)
		r = builder.BuildModule();
	if (r >= 0)
	{
		m_gameInstance.CacheData(builder);
		m_gameInstance.CreateObject(m_scriptContext);
		m_gameInstance.CallOnStart(m_scriptContext);
	}*/

	return true;
}

void TestApp::OnShutdown()
{
	/*m_gameInstance.CallOnEnd(m_scriptContext);
	m_scriptContext->Release();*/
	Runtime::OnShutdown();
}

void TestApp::OnTick(f64 dt)
{
	Runtime::OnTick(dt);
	//m_gameInstance.CallOnUpdate(m_scriptContext, dt);
}

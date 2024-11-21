#pragma once
#include "Engine/Runtime/Runtime.h"
#include "Engine/Engine.h"
#include "Logger/Logger.h"
#include "Logger/Sinks/DebugSink.h"
#include "Logger/Sinks/ConsoleSink.h"
#include "Utils/MessageBox.h"
#include "Config/Config.h"
#include "Utils/Singleton.h"
#include "Game/ScriptableGameInstance.h"
#include <libassert/assert.hpp>
#include <filesystem>
#include <External/AngelScript/AddOns/scriptbuilder/scriptbuilder.h>


class TestApp : public Ryu::Engine::Runtime
{
public:
	RYU_LOG_CATEGORY(TestApp);

	bool OnInit() override
	{
		if (!Runtime::OnInit())
			return false;

		const Ryu::Scripting::ScriptEngine& engine = GetScriptEngine();
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
		}

		return true;
	}


	void OnShutdown() override 
	{
		m_gameInstance.CallOnEnd(m_scriptContext);
		m_scriptContext->Release();
		Runtime::OnShutdown();
	}
	
	void OnTick(f64 dt) override
	{
		Runtime::OnTick(dt);
		m_gameInstance.CallOnUpdate(m_scriptContext, dt);
	}

private:
	Ryu::Game::ScriptableGameInstance m_gameInstance;
	asIScriptContext* m_scriptContext{ nullptr };
};

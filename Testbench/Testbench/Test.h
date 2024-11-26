#pragma once
#include "Engine/Runtime/Runtime.h"
#include "Logger/LogCategory.h"
#include "Game/ScriptableGameInstance.h"


class TestApp : public Ryu::Engine::Runtime
{
public:
	RYU_LOG_CATEGORY(TestApp);

	bool OnInit() override;
	void OnShutdown() override;
	void OnTick(f64 dt) override;	

private:
	Ryu::Game::ScriptableGameInstance m_gameInstance;
	asIScriptContext* m_scriptContext{ nullptr };
};

#pragma once
#include "Testbench/Test.h"
#include "Engine/Game/IGameModule.h"

class TestGameModule final : public Ryu::Engine::IGameModule
{
public:
	Ryu::App::Application* CreateApplication() override
	{
		return new TestApp();
	}

	constexpr const char* GetName() const override
	{
		return "Test App";
	}
};

RYU_IMPLEMENT_GAME_MODULE(TestGameModule)

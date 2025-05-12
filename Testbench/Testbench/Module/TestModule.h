#pragma once
#include "Engine/Game/IGameModule.h"

class TestModule final : public Ryu::Engine::IGameModule
{
public:
	Ryu::App::Application* CreateApplication() override;
	constexpr const char* GetName() const override;
};

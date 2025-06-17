#pragma once
#include "Game/IGameModule.h"

class TestModule final : public Ryu::Game::IGameModule
{
public:
	std::shared_ptr<Ryu::App::App> CreateApplication(std::shared_ptr<Ryu::Window::Window> window) override;
	constexpr const char* GetName() const override;
};

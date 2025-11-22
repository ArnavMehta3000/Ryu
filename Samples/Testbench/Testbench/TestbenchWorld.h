#pragma once
#include "Game/World/World.h"

class TestbenchWorld : public Ryu::Game::World
{
public:
	TestbenchWorld() : Ryu::Game::World("TestbenchWorld") {}
	~TestbenchWorld() = default;

protected:
	void OnCreate() override;
	void OnDestroy() override;
};

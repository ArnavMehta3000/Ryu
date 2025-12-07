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
	void OnTick(const Ryu::Utils::FrameTimer& timer) override;

#if defined(RYU_WITH_EDITOR)
	void OnImGuiRender() override;
#endif

private:
	Ryu::Utils::FrameTimer m_timer;
};

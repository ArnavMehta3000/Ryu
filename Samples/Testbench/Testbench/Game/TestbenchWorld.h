#pragma once
#include "Game/World/World.h"
#include "Game/World/Entity.h"

namespace Ryu::Game
{
	class Entity;
	struct Transform;
}

namespace Ryu::Game { class InputManager; }

class TestbenchWorld : public Ryu::Game::World
{
public:
	TestbenchWorld() : Ryu::Game::World("Testbench World") {}
	~TestbenchWorld() = default;

protected:
	void OnCreate() override;
	void OnDestroy() override;
	void OnTick(const Ryu::Utils::FrameTimer& timer) override;

#if defined(RYU_WITH_EDITOR)
	void OnImGuiRender() override;
#endif

private:
	Ryu::Game::InputManager* m_inputManager{ nullptr };
	Ryu::Utils::FrameTimer    m_timer;
	Ryu::Game::Entity         m_mainCamera;
	//Ryu::Game::Entity         m_meshEntity;
	f32 m_speed = 0.0f;
};

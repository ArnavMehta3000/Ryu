#pragma once
#include "Application/App/Application.h"
#include "Game/World/WorldManager.h"
#include "Testbench/Input/GameInput.h"

class TestbenchApp : public Ryu::App::App
{
public:
	explicit TestbenchApp(const std::shared_ptr<Ryu::Window::Window>& window);

	bool OnInit() override;
	void OnShutdown() override;
	void OnTick(const Ryu::Utils::FrameTimer& timer) override;

	[[nodiscard]] inline Ryu::Game::WorldManager* GetWorldManager() noexcept override { return &m_worldManager; }

private:
	void TestSerialization();

private:
	GInput                  m_gameInput;
	Ryu::Game::WorldManager m_worldManager;
	Ryu::Utils::FrameTimer  m_timer;
};

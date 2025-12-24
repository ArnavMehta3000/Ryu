#pragma once
#include "Application/App/Application.h"
#include "Game/World/WorldManager.h"
#include "Application/Event/ScopedListener.h"
#include "Testbench/Input/GameInput.h"

class TestbenchApp : public Ryu::App::App
{
	using ScopedKeyEventListener = Ryu::Event::ScopedListener<Ryu::Window::KeyEvent>;
public:
	explicit TestbenchApp(const std::shared_ptr<Ryu::Window::Window>& window);

	bool OnInit() override;
	void OnShutdown() override;
	void OnTick(const Ryu::Utils::FrameTimer& timer) override;

	[[nodiscard]] inline Game::WorldManager* GetWorldManager() noexcept override { return &m_worldManager; }

private:
	void TestSerialization();

private:
	Ryu::Game::WorldManager m_worldManager;
	GInput                  m_gameInput;
	ScopedKeyEventListener  m_keyListener;
	Ryu::Utils::FrameTimer  m_timer;
};

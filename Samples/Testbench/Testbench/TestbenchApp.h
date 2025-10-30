#pragma once
#include "App/Application.h"
#include "Game/World/World.h"
#include "Event/ScopedListener.h"
#include "Testbench/Input/GameInput.h"

class TestbenchApp : public Ryu::App::App
{
	using ScopedKeyEventListener = Ryu::Event::ScopedListener<Ryu::Window::KeyEvent>;
public:
	explicit TestbenchApp(std::shared_ptr<Ryu::Window::Window> window);

	bool OnInit() override;
	void OnShutdown() override;
	void OnTick(const Ryu::Utils::TimeInfo& timeInfo) override;

private:
	void TestSerialization();
	void TestDeserialization();

private:
	Ryu::Game::World       m_world;
	GInput                 m_gameInput;
	ScopedKeyEventListener m_keyListener;
	Ryu::Utils::TimeInfo   m_timeInfo;
};

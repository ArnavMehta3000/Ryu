#pragma once
#include "App/Application.h"
#include "Testbench/Input/GameInput.h"

class TestbenchApp : public Ryu::App::App
{
public:
	explicit TestbenchApp(std::shared_ptr<Ryu::Window::Window> window);

	bool OnInit() override;
	void OnShutdown() override;
	void OnTick(const Ryu::Utils::TimeInfo& timeInfo) override;
private:
	GInput m_gameInput;
};

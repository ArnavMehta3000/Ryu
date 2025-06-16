#pragma once
#include "App/Application.h"
#include "Logger/LogCategory.h"

class TestbenchApp : public Ryu::App::Application
{
	RYU_LOG_DECLARE_CATEGORY(TestbenchApp);

public:
	bool OnInit() override;
	void OnShutdown() override;
	void OnTick(const Ryu::Utils::TimeInfo& timeInfo) override;

private:
	void OnWindowKeyPressedEvent(const Elos::Event::KeyPressed& e) override;
};

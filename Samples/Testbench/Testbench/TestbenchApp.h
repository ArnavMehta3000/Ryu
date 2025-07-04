#pragma once
#include "App/Application.h"
#include "Logger/LogCategory.h"

class TestbenchApp : public Ryu::App::App
{
	RYU_LOG_DECLARE_CATEGORY(TestbenchApp);

public:
	explicit TestbenchApp(std::shared_ptr<Ryu::Window::Window> window);

	bool OnInit() override;
	void OnShutdown() override;
	void OnTick(const Ryu::Utils::TimeInfo& timeInfo) override;
};

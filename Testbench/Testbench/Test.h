#pragma once
#include "App/Application.h"
#include "Logger/LogCategory.h"

class TestApp : public Ryu::App::Application
{
public:
	RYU_LOG_DECLARE_CATEGORY(TestApp);

	bool OnInit() override;
	void OnShutdown() override;
	void OnTick(const Ryu::Utils::TimeInfo& timeInfo) override;
};

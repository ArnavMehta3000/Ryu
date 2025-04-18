#pragma once
#include "App/Application.h"
#include "Logger/LogCategory.h"

class TestApp : public Ryu::App::Application
{
public:
	RYU_LOG_CATEGORY(TestApp);

	bool OnInit() override;
	void OnShutdown() override;
	void OnTick(f64 dt) override;	
};

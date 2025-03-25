#pragma once
#include "Engine/Runtime/Runtime.h"
#include "Logger/LogCategory.h"

class TestApp : public Ryu::Engine::Runtime
{
public:
	RYU_LOG_CATEGORY(TestApp);

	bool OnInit() override;
	void OnShutdown() override;
	void OnTick(f64 dt) override;	
};

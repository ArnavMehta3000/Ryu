#pragma once
#include "Engine/Runtime/Runtime.h"
#include "Engine/Engine.h"
#include "Logger/Logger.h"
#include "Logger/Sinks/DebugSink.h"
#include "Logger/Sinks/ConsoleSink.h"
#include "Utils/MessageBox.h"
#include "Config/Config.h"
#include "Utils/Singleton.h"
#include <libassert/assert.hpp>

class TestApp : public Ryu::Engine::Runtime
{
public:
	RYU_LOG_CATEGORY(TestApp);

	bool OnInit() override { return  Runtime::OnInit(); }
	void OnShutdown() override { Runtime::OnShutdown(); }
	void OnTick(f64 dt) override { Runtime::OnTick(dt); }
};

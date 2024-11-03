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

	bool OnInit() override { return  true; }
	void OnShutdown() override {}
	void OnTick(MAYBE_UNUSED f64 dt) override {}
};

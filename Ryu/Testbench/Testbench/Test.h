#pragma once
#include "App/Application.h"
#include "Engine/Engine.h"
#include "Logger/Logger.h"
#include "Logger/Sinks/DebugSink.h"
#include "Logger/Sinks/ConsoleSink.h"
#include "Utils/MessageBox.h"
#include "Config/Config.h"
#include "Utils/Singleton.h"
#include <libassert/assert.hpp>

class TestApp : public Ryu::App::Application
{
public:
	RYU_LOG_CATEGORY(TestApp);

	void LocalCallback(u32 value)
	{
		LOG_INFO(TestAppLog, "LocalCallback {}", value);
	}
};

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
	LOG_CATEGORY(TestApp);
};

struct MyConfig : public Ryu::Config::IConfig<MyConfig>
{
	int x = 3;
	int y = 5;
};

class TestSingleton
{
public:
	TestSingleton() {}
	RYU_DECLARE_SINGLETON(TestSingleton);

public:
	i32 X = 0;
};

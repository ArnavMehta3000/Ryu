#include <Windows.h>
#include "App/Application.h"
#include "Engine/Engine.h"
#include "Logger/Logger.h"
#include "Logger/Sinks/DebugSink.h"
#include "Logger/Sinks/ConsoleSink.h"
#include "Utils/MessageBox.h"
#include "libassert/assert.hpp"
#include "Config/Config.h"

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


int WINAPI wWinMain(
	_In_     MAYBE_UNUSED HINSTANCE hInstance,
	_In_opt_ MAYBE_UNUSED HINSTANCE hPrevInstance,
	_In_     MAYBE_UNUSED LPWSTR    lpCmdLine,
	_In_     MAYBE_UNUSED int       nCmdShow)
{
	using namespace Ryu;

	
	auto& logger = Logging::Logger::Get();
	logger.AddSink(std::make_unique<Logging::DebugSink>());
	logger.AddSink(std::make_unique<Logging::ConsoleSink>());
	logger.SetOnFatalCallback([](Logging::LogLevel level, const Logging::LogMessage& message)
	{
		Utils::MessageBoxDesc desc;
		desc.Title        = EnumToString(level);
		desc.Title       += " Error";
		desc.Text         = message.Message;
		desc.Flags.Button = Utils::MessagBoxButton::Ok;
		desc.Flags.Icon   = Utils::MessageBoxIcon::Error;

		Utils::ShowMessageBox(desc);
		PANIC("FATAL PROBLEMO");
	});

	 
	{
		LOG_TRACE(TestApp::TestAppLog, "This is the {} {} {}", 1, "trace", "test");
		LOG_DEBUG(TestApp::TestAppLog, "This is the {} {} {}", 2, "debug", "test");
		LOG_INFO(TestApp::TestAppLog, "This is the {} {} {}", 3, "debug", "test");
		LOG_WARN(TestApp::TestAppLog, "This is the {} {} {}", 4, "warn", "test");
		LOG_ERROR(TestApp::TestAppLog, "This is the {} {} {}", 5, "error", "test");
		LOG_FATAL(TestApp::TestAppLog, "This is the {} {} {}", 6, "fatal", "test");
	}

	Engine::Engine::Get()
		.SetCommandLine(lpCmdLine)
		.Run();

	return 0;
}

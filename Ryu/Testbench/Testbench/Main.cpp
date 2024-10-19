#include <Windows.h>
#include "Test.h"

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

	Engine::Engine engine;
	engine.SetCommandLine(lpCmdLine);
	engine.SetApp(std::make_shared<TestApp>());
	engine.Run();

	return 0;
}

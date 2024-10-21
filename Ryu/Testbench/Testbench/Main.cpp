#include <Windows.h>
#include "Test.h"

void GlobalCallback(u32 value)
{
	LOG_INFO(TestApp::TestAppLog, "GlobalCallback: {}", value);
}

int WINAPI wWinMain(
	_In_     MAYBE_UNUSED HINSTANCE hInstance,
	_In_opt_ MAYBE_UNUSED HINSTANCE hPrevInstance,
	_In_     MAYBE_UNUSED LPWSTR    lpCmdLine,
	_In_     MAYBE_UNUSED int       nCmdShow)
{
	using namespace Ryu;

	// Creating the engine object initializes all core subsystems
	Engine::Engine engine;
	auto app = std::make_shared<TestApp>();

	Events::Event<u32>::DelegateHandle lambdaHandle = engine.m_OnEvent.Add([](u32 value)
		{
			LOG_INFO(TestApp::TestAppLog, "LambdaCallback: {}", value);
		});

	Events::Event<u32>::DelegateHandle globalHandle = engine.m_OnEvent.Add(&GlobalCallback);
	Events::Event<u32>::DelegateHandle localHandle = engine.m_OnEvent.Add(app.get(), &TestApp::LocalCallback);

	engine.m_OnEvent.Remove(localHandle);
	
	Logging::Logger* logger = Logging::Logger::Get();
	logger->AddSink(std::make_unique<Logging::DebugSink>());
	logger->AddSink(std::make_unique<Logging::ConsoleSink>());
	logger->SetOnFatalCallback([](Logging::LogLevel level, const Logging::LogMessage& message)
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

	engine.SetCommandLine(lpCmdLine);
	engine.SetApp(app);
	engine.Run();

	return 0;
}

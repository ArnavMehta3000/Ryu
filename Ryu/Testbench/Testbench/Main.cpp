#include <Windows.h>
#include "Test.h"

using namespace Ryu;

RYU_SINGLETON_IMPL(TestSingleton);

int WINAPI wWinMain(
	_In_     MAYBE_UNUSED HINSTANCE hInstance,
	_In_opt_ MAYBE_UNUSED HINSTANCE hPrevInstance,
	_In_     MAYBE_UNUSED LPWSTR    lpCmdLine,
	_In_     MAYBE_UNUSED int       nCmdShow)
{
	auto& logger = Logging::Logger::Get();
	logger.AddSink(std::make_unique<Logging::DebugSink>());
	logger.AddSink(std::make_unique<Logging::ConsoleSink>());
	logger.SetOnFatalCallback([](Logging::LogLevel level, const Logging::LogMessage& message)
		{
			Utils::MessageBoxDesc desc;
			desc.Title = EnumToString(level);
			desc.Title += " Error";
			desc.Text = message.Message;
			desc.Flags.Button = Utils::MessagBoxButton::Ok;
			desc.Flags.Icon = Utils::MessageBoxIcon::Error;

			Utils::ShowMessageBox(desc);
			PANIC("FATAL PROBLEMO");
		});


	//Engine::Engine::Get()
	//	.SetCommandLine(lpCmdLine)
	//	.SetApp(std::make_shared<TestApp>())
	//	.Run();
	
	TestSingleton t{};

	TestSingleton::Get()->X = 10;
	TestSingleton* x = Utils::SingletonRegistry::Get<TestSingleton>();
	x->X = 20;

	auto& all = Ryu::Utils::SingletonRegistry::GetAllRegisteredSingletons();
	for (auto& [hash, entry] : all)
	{
		LOG_INFO(USE_LOG_CATEGORY(TestApp::TestApp), "{}", entry.Name);
	}

	return 0;
}

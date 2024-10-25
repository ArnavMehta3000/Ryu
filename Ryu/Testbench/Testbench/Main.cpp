#include <Windows.h>
#include "Test.h"
#include "Graphics/Config.h"

int WINAPI wWinMain(
	_In_     MAYBE_UNUSED HINSTANCE hInstance,
	_In_opt_ MAYBE_UNUSED HINSTANCE hPrevInstance,
	_In_     MAYBE_UNUSED LPWSTR    lpCmdLine,
	_In_     MAYBE_UNUSED int       nCmdShow)
{
	using namespace Ryu;

	// Set the working API
	Graphics::GraphicsConfig::Get().GraphicsAPI = Graphics::API::DirectX11;

	// Creating the engine object initializes all core subsystems
	Engine::Engine engine;
	
	Logging::Logger* logger = Logging::Logger::Get();
	logger->AddSink(std::make_unique<Logging::DebugSink>());
	logger->AddSink(std::make_unique<Logging::ConsoleSink>());
	logger->SetOnFatalCallback([](Logging::LogLevel level, const Logging::LogMessage& message)
	{
		Utils::MessageBoxDesc desc;
		desc.Title        = Ryu::EnumToString(level);
		desc.Title       += " Error";
		desc.Text         = message.Message;
		desc.Flags.Button = Utils::MessagBoxButton::Ok;
		desc.Flags.Icon   = Utils::MessageBoxIcon::Error;

		Utils::ShowMessageBox(desc);
		PANIC("FATAL PROBLEMO");
	});

	engine.SetCommandLine(lpCmdLine);
	engine.SetApp(std::make_shared<TestApp>());
	engine.Run();

	return 0;
}

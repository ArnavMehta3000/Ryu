#include <Windows.h>
#include "Test.h"
#include "Graphics/Config.h"

int WINAPI wWinMain(
	_In_     MAYBE_UNUSED HINSTANCE hInstance,
	_In_opt_ MAYBE_UNUSED HINSTANCE hPrevInstance,
	_In_     MAYBE_UNUSED LPWSTR    lpCmdLine,
	_In_     MAYBE_UNUSED int       nCmdShow)
{
	using namespace Ryu::Engine;
	using namespace Ryu::Engine;
	using namespace Ryu::Graphics;
	using namespace Ryu::Logging;
	using namespace Ryu::Config;

	// Set the working API
	GraphicsConfig::Get().GraphicsAPI = API::DirectX11 ;

	// Creating the engine object initializes all core subsystems
	Engine engine;
	SetUpDefaultLogger();

	engine.SetCommandLine(lpCmdLine);
	engine.SetRuntime(std::make_shared<TestApp>());
	engine.Run();

	return 0;
}

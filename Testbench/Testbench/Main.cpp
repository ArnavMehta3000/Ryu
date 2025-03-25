#include <Windows.h>
#include "Engine/Engine.h"
#include "GraphicsRHI/Config.h"
#include "Test.h"

#if defined (RYU_BUILD_DEBUG)
constexpr bool g_isDebug = true;
#else
constexpr bool g_isDebug = false;
#endif

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

	Logging::SetUpDefaultLogger(g_isDebug);  // Create a console logger only in debug builds

	engine.SetCommandLine(lpCmdLine);
	engine.SetRuntime(std::make_shared<TestApp>());
	engine.Run();

	return 0;
}

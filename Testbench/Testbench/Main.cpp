#include <Windows.h>
#include "Engine/Engine.h"
#include "Config/ConfigManager.h"
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

	Logging::SetUpDefaultLogger(g_isDebug);  // Create a console logger only in debug builds
	Config::ConfigManager::Get().Initialize(RYU_ROOT_CONFIG_DIR "/Testbench");
	
	Engine::Engine& engine = Engine::Engine::Get();

	engine.SetCommandLine(lpCmdLine);
	engine.SetRuntime(std::make_shared<TestApp>());
	engine.Run();

	Config::ConfigManager::Get().SaveAll();
	return 0;
}

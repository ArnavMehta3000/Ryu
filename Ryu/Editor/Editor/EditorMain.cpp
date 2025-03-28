#include "Editor/Application/EditorApp.h"
#include "Engine/Engine.h"
#include "GraphicsRHI/Config.h"
#include <Windows.h>

int WINAPI wWinMain(
	_In_     MAYBE_UNUSED HINSTANCE hInstance,
	_In_opt_ MAYBE_UNUSED HINSTANCE hPrevInstance,
	_In_     MAYBE_UNUSED LPWSTR    lpCmdLine,
	_In_     MAYBE_UNUSED int       nCmdShow)
{
	using namespace Ryu;

	Logging::SetUpDefaultLogger(false);
	Config::ConfigManager::Get().Initialize(RYU_ROOT_CONFIG_DIR "/Editor");

	Engine::Engine& engine = Engine::Engine::Get();

	engine.SetCommandLine(lpCmdLine);
	engine.SetRuntime(std::make_shared<Editor::EditorApp>());
	engine.Run();

	Config::ConfigManager::Get().SaveAll();
	return 0;
}

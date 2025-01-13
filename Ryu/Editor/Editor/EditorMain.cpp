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
	
	// Set the working API
	Graphics::GraphicsConfig::Get().GraphicsAPI = Graphics::API::DirectX12;
	Graphics::GraphicsConfig::Get().EnableDebugLayer = true;

	// Creating the engine object initializes all core subsystems
	Engine::Engine engine;
	Logging::SetUpDefaultLogger();

	engine.SetCommandLine(lpCmdLine);
	engine.SetRuntime(std::make_shared<Editor::EditorApp>());
	engine.Run();

	return 0;
}

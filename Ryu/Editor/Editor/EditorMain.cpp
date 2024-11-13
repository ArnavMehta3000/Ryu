#include <Windows.h>
#include "Editor/Application/EditorApp.h"
#include "Engine/Engine.h"
#include "Graphics/Config.h"
//#include "Scripting/ScriptEngine.h"

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
	Logging::SetUpDefaultLogger();

	engine.SetCommandLine(lpCmdLine);
	engine.SetRuntime(std::make_shared<Editor::EditorApp>());

	//Scripting::ScriptEngine se;
	//se.Init();

	engine.Run();

	return 0;
}

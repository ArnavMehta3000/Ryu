#include "Engine/EngineMain.h"
#include "Editor/Application/EditorApp.h"

using namespace Ryu;
using namespace Ryu::Editor;


RYU_MAIN()
{
	RYU_DBG_TRACK_MEM();

	// Create editor window
	Window::Window::Config windowConfig { .Title = "Ryu Editor" };
	auto editorWindow = std::make_shared<Window::Window>(windowConfig);
	App::App::InitWindow(*editorWindow);

	// Create editor application
	auto editorApp = std::make_shared<Editor::EditorApp>(editorWindow);
	Engine::Engine::Get().RunApp(editorApp);

	return 0;
}

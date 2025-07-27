#include "Engine/EngineMain.h"
#include "Editor/Application/EditorApp.h"
#include "Logger/Assert.h"

using namespace Ryu;
using namespace Ryu::Editor;


RYU_MAIN()
{
	RYU_DBG_TRACK_MEM();
	//RYU_DBG_TRACK_MEM_LEAK(928);

	try
	{
		// Create editor window
		Window::Window::Config windowConfig{ .Title = "Ryu Editor" };
		auto editorWindow = std::make_shared<Window::Window>(windowConfig);
		App::App::InitWindow(*editorWindow);

		// Create editor application
		auto editorApp = std::make_shared<Editor::EditorApp>(editorWindow);
		Engine::Engine::Get().RunApp(editorApp);
	}
	catch (const Exception& e)
	{
		RYU_LOG_FATAL(LogAssert, "{}", e.what());
	}

	return 0;
}

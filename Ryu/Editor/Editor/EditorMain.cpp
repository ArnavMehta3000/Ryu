#include "Common/Assert.h"
#include "Engine/Setup/EngineMain.h"
#include "Engine/Setup/Setup.h"
#include "Editor/Application/EditorApp.h"

using namespace Ryu;
using namespace Ryu::Editor;

RYU_MAIN()
{
	RYU_DBG_TRACK_MEM();

	try
	{
		if (!Engine::Setup())
		{
			Engine::Shutdown();
			return 1;
		}

		// Create editor window
		Window::Window::Config windowConfig{ .Title = "Ryu Editor" };
		auto editorWindow = std::make_shared<Window::Window>(windowConfig);
		App::App::InitWindow(*editorWindow);

		// Create editor application
		auto editorApp = std::make_shared<Editor::EditorApp>(editorWindow);
		Engine::Engine::Get().RunApp(editorApp);

		Engine::Shutdown();
	}
	catch (const AssertException& e)
	{
		RYU_LOG_FATAL("{}", e.what());

		Engine::Shutdown();
	}

	return 0;
}

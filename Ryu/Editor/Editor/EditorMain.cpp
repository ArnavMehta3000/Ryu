#include "Engine/Setup/EngineMain.h"
#include "Engine/Setup/Setup.h"
#include "Editor/Application/EditorApp.h"
#include <print>
#include <numeric>

using namespace Ryu;
using namespace Ryu::Editor;

using namespace std::chrono_literals;

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
		Window::Window::Config windowConfig
		{
			.Title             = "Ryu Editor",
			.CanBorderlessDrag = true,
			.Type              = Window::WindowType::Windowed,
		};

		auto editorWindow = std::make_shared<Window::Window>(windowConfig);
		App::App::InitWindow(*editorWindow);

		// Create editor application
		auto editorApp = std::make_shared<Editor::EditorApp>(editorWindow);
		Engine::Engine::Get().RunApp(editorApp, editorApp.get());

		Engine::Shutdown();
	}
	catch (const AssertException& e)
	{
		RYU_LOG_FATAL("{}", e.what());

		Engine::Shutdown();
	}

	return 0;
}

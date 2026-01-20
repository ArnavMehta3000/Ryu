#include "Engine/Setup/EngineMain.h"
#include "Engine/Setup/Setup.h"
#include "Core/Config/CmdLine.h"
#include "Editor/Application/EditorApp.h"
#include <print>
#include <numeric>

using namespace Ryu;
using namespace Ryu::Editor;
using namespace Ryu::Config;

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

		const EditorConfig editorConfig
		{
			.WindowConfig =
			{
				.Title             = "Ryu Editor",
				.CanBorderlessDrag = true,
				.Type              = Window::WindowType::Windowed,
			},
			.GameModulePath  = CmdLine::Get().GetCVarValue("Game.ModulePath"),
			.EnableHotReload = true,
		};

		auto editorApp = std::make_shared<Editor::EditorApp>(editorConfig);
		Engine::Engine::Get().RunApp(editorApp.get(), editorApp->GetRendererHook());

		Engine::Shutdown();
	}
	catch (const AssertException& e)
	{
		RYU_LOG_FATAL("{}", e.what());
		Engine::Shutdown();
		return 1;
	}

	return 0;
}

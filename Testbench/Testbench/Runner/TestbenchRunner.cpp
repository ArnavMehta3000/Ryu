#include "Engine/Entry/EntryPoint.h"
#include "Testbench/TestApp.h"

/*
* This script/project demos how to run the game project as a DLL
*	- Build with --ryu-game-as-dll=y which will make RyuTestbench be built as a dll
*	- The runner is an example main that shows how to load the DLL
*	- This is how the editor will load the game module
*/

RYU_MAIN()
{
	RYU_DBG_TRACK_MEM();

	Ryu::Engine::Runner
	{
		.ProjectDir = fs::path(RYU_ROOT_DIR) / "Testbench",
		.GameDLLPath = std::filesystem::current_path() / "RyuTestbench.dll"
	}.Run();
	return 0;
}

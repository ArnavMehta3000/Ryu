#include "Engine/EntryPoint.h"
#include "Module/TestGameModule.h"

#if defined(RYU_BUILD_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif


#if !defined(RYU_GAME_AS_DLL)
	RYU_USE_APP(TestApp)
#endif

RYU_MAIN()
{
#if defined(RYU_BUILD_DEBUG)  // Enable memory leak checking
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Ryu::Engine::Runner
	{
		.ProjectDir = RYU_ROOT_DIR "/Testbench"
	}.Run();

	return 0;
}

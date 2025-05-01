#include "Engine/EntryPoint.h"
#include "Test.h"

#if defined(RYU_BUILD_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

RYU_USE_APP(TestApp)

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

#include "Engine/EntryPoint.h"
#include "Test.h"

RYU_USE_APP(TestApp)

RYU_MAIN()
{
	Ryu::Engine::Runner
	{
		.ProjectDir = RYU_ROOT_DIR "/Testbench"
	}.Run();

	return 0;
}

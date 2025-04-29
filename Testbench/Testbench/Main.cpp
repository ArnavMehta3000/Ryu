#include "Engine/EntryPoint.h"
#include "Test.h"

RYU_USE_APP(TestApp)

RYU_MAIN()
{
	Ryu::Engine::Runner runner;
	runner.ProjectDir = RYU_ROOT_DIR "/Testbench";

	runner.Run();

	return 0;
}

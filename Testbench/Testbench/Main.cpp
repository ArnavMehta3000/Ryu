#include "Engine/Entry/EntryPoint.h"
#include "Testbench/TestApp.h"

RYU_USE_APP(TestApp)

RYU_MAIN()
{
	RYU_DBG_TRACK_MEM();
	_CrtSetBreakAlloc(4870);
	Ryu::Engine::Run();

	return 0;
}

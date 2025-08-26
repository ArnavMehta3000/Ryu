#pragma  once
#include "Engine/Engine.h"

#if defined(RYU_BUILD_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
// Use the CRT Debug library to track memory leaks
#define RYU_DBG_TRACK_MEM() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
#define RYU_DBG_TRACK_MEM_LEAK(LeakNum) _CrtSetBreakAlloc(LeakNum)
#else
#define RYU_DBG_TRACK_MEM() do {} while(false);
#define RYU_DBG_TRACK_MEM_LEAK(LeakNum) do {} while(false);
#endif

// WinMain
#define RYU_WIN_MAIN()                             \
int WINAPI wWinMain(                               \
	_In_     [[maybe_unused]] HINSTANCE hInstance,     \
	_In_opt_ [[maybe_unused]] HINSTANCE hPrevInstance, \
	_In_     [[maybe_unused]] LPWSTR    lpCmdLine,     \
	_In_     [[maybe_unused]] int       nCmdShow)

// Default main
#define RYU_DEF_MAIN() int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])

// Use dummy main when we are building with editor (for standalone main func) and not in editor (otherwise the editor gets the dummy main)
#if defined(RYU_WITH_EDITOR) && !defined(RYU_IS_EDITOR)
	#define RYU_MAIN() int DummyMain()
#else
	#ifdef RYU_BUILD_DEBUG
		// Use default main in debug mode (enables native console)
		#define RYU_MAIN() RYU_DEF_MAIN()
	#else
		// Use WinMain in non-debug modes (manually create console via logger)
		#define RYU_MAIN() RYU_WIN_MAIN()
	#endif
#endif

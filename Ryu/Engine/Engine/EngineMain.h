#pragma  once
#include "Engine/Engine.h"

#if defined(RYU_BUILD_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
// Use the CRT Debug library to track memory leaks
#define RYU_DBG_TRACK_MEM() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
#else
#define RYU_DBG_TRACK_MEM() do {} while(false);
#endif

// WinMain
#define RYU_WIN_MAIN()                             \
int WINAPI wWinMain(                               \
	_In_     MAYBE_UNUSED HINSTANCE hInstance,     \
	_In_opt_ MAYBE_UNUSED HINSTANCE hPrevInstance, \
	_In_     MAYBE_UNUSED LPWSTR    lpCmdLine,     \
	_In_     MAYBE_UNUSED int       nCmdShow)

// Default main
#define RYU_DEF_MAIN() int main(MAYBE_UNUSED int argc, MAYBE_UNUSED char *argv[])

#ifdef RYU_BUILD_DEBUG
// Use default main in debug mode (enables native console)
#define RYU_MAIN() RYU_DEF_MAIN()
#else
// Use WinMain in non-debug modes (manually create console via logger)
#define RYU_MAIN() RYU_WIN_MAIN()
#endif

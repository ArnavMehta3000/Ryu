#include <Windows.h>
#include <Core/Log/Logger.h>
#include <Engine/Internal/Log.h>

BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
)
{
	UNREFERENCED_PARAMETER(hinstDLL);

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		RYU_ENGINE_INITIALIZE_LOGGER("Engine");
		break;
	}

	case DLL_THREAD_ATTACH:  // Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:  // Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:

		if (lpvReserved != nullptr)
		{
			break; // Do not do cleanup if process termination scenario
		}

		RYU_ENGINE_SHUTDOWN_LOGGER();
		break;
	}
	return TRUE;
}

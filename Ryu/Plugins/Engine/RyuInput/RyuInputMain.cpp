#include <Windows.h>
#include <Plugins/Engine/RyuInput/Generated/RyuInputLog.h>

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
		Ryu::Logger::Initialize("RyuInput");
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

		Ryu::Logger::Shutdown();
		break;
	}
	return TRUE;
}

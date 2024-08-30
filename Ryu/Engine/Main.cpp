#include <Windows.h>
#include <Core/Log/Logger.h>
#include <Engine/Log.h>

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
		Ryu::Logger::Initialize("Engine");

		RYU_ENGINE_TRACE("This is a trace message");
		RYU_ENGINE_INFO("This is an info message");
		RYU_ENGINE_WARN("This is a warning message");
		RYU_ENGINE_ERROR("This is an error message");
		RYU_ENGINE_FATAL("This is a fatal error message");
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

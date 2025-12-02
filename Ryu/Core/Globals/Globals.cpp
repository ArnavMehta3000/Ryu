#include "Globals/Globals.h"
#include "Utils/ServiceLocator.h"
#include <Windows.h>

namespace Ryu::Globals
{
#if defined (RYU_BUILD_DEBUG)
	constexpr bool g_isDebug = true;
#else
	constexpr bool g_isDebug = false;
#endif

	bool IsDebuggerAttached()
	{
		return ::IsDebuggerPresent();
	}

	Utils::ServiceLocator& GetServiceLocator()
	{
		static Utils::ServiceLocator serviceLocator;
		return serviceLocator;
	}
}

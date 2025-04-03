#include "Common/Globals.h"
#include <Windows.h>

namespace Ryu::Common::Globals
{
#if defined (RYU_BUILD_DEBUG)
	constexpr const bool g_isDebug = true;
#else
	constexpr const bool g_isDebug = false;
#endif
	
	bool IsDebuggerAttached()
	{
		return ::IsDebuggerPresent();
	}
}

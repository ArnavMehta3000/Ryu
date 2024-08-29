#pragma once
#include <CoreDLLDefines.h>

namespace Ryu
{
	class Engine;

	extern "C"
	{
		RYU_API extern Engine* g_engine;
	}
}
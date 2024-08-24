#pragma once
#include <CoreDLLDefines.h>

namespace Ryu
{
	class Logger
	{
	public:
		static RYU_API void Initialize();
		static RYU_API void Shutdown();
	};
}
